/// File: CockpitPanel.cxx

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdint.h>
using namespace std;

#include "CockpitNodeInternal.hxx"
#include "CockpitIndicators.hxx"
#include "CockpitPanel.hxx"

extern unsigned int logLevel;

typedef enum {
  chstate_OpenDelay,
  chstate_Identify,
  chstate_WaitReq,
  chstate_WaitResponse,
  chstate_ReqStatus,
} ChannelState_t;

typedef struct CockpitNodeComm {
  char commPortPath[32];
  int commPortFd;
  ChannelState_t state;
  unsigned long delayTime;
  CockpitNode *node;
  unsigned int retry;
  unsigned char isOpenErrorReported;
  unsigned char isNodeModified;
  const char *pLastAction;
  char lastMessageReceived[64];
  char lastMessageSent[64];
} CockpitNodeComm_t;

static CockpitNodeComm_t cockpitChannel[maxIndicatorTable];

const char *commChannelGetPortPath(CockpitNodeComm_t *pCommChannel)
{
  return pCommChannel->commPortPath;
}

const char *getCockpitStateName(ChannelState_t state)
{
  switch (state)
  {
    case chstate_OpenDelay:
      return "<OpenDelay>";
    case chstate_Identify:
      return "<Identify>";
    case chstate_WaitReq:
      return "<WaitReq>";
    case chstate_WaitResponse:
      return "<WaitResponse>";
    case chstate_ReqStatus:
      return "<ReqStatus>";
    default:
      return "<state:unknown>";
  }
}

static void commChannelSetState(CockpitNodeComm_t *pCommChannel, ChannelState_t newState)
{
  if (pCommChannel->state != newState)
  {
    if (logLevel > 2)
    {
      cout << "Info: Node[";
      if (pCommChannel->node != NULL)
	cout << pCommChannel->node->getCockpitNodeName() << ":";
      cout << pCommChannel->commPortPath << "]: State from " << getCockpitStateName(pCommChannel->state);
      cout << " to " << getCockpitStateName(newState) << endl;
    }

    pCommChannel->state = newState;
  }
}

/// This time elapse implementation slips a little because it is simple, but that is ok for this application.
static unsigned long GetMillisecondTime(void)
{
  struct timeval tv;

  if (gettimeofday(&tv, 0) != 0)
  {
    cout << "Error: Problem getting time-of-day value" << endl;
    exit(2);
    return 0;
  }
  return (unsigned long)((tv.tv_sec * 1000ul) + (tv.tv_usec / 1000ul));
}

static unsigned long GetMillisecondsElapsed(void)
{
  static unsigned long lastMillisecondTime = 0;
  unsigned long millisecondTime;
  unsigned elapsedMilliseconds;

  millisecondTime = GetMillisecondTime();
  if (millisecondTime == 0)
  {
    elapsedMilliseconds = 0;
  }
  else
  {
    elapsedMilliseconds = millisecondTime - lastMillisecondTime;
    if (elapsedMilliseconds < 10ul)
      elapsedMilliseconds = 0;
    else
      lastMillisecondTime = millisecondTime;
  }
  return elapsedMilliseconds;
}

char fStringStartsWith(const char *pS1, const char *pS2)
{
  while (*pS2 != '\0' && *pS2 == *pS1)
  {
    pS1 += 1;
    pS2 += 1;
  }
  return *pS2 == '\0';
}


/// Open and configure the specified serial channel.  Return FALSE if not possible.
///
static char isCommChannelOpen(CockpitNodeComm_t *pCommChannel)
{
  int fd;
  int result;
  struct termios config;

  fd = open(pCommChannel->commPortPath, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if (fd > 0)
  {
    result = tcgetattr(fd, &config);
    if (result >= 0)
    {
      config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

      config.c_oflag = 0;

      config.c_lflag &= ~(ECHO | ECHONL | IEXTEN | ISIG);
      config.c_lflag |= ICANON;

      //      config.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
      config.c_cflag = B38400 | CS8 | CLOCAL | CREAD;

      tcflush(fd, TCIFLUSH);
      tcsetattr(fd, TCSANOW, &config);

      pCommChannel->commPortFd = fd;
      return 1;
    }
  }
  return 0;
}

/// Copy message returning the destination end pointer.
static char *msgcpy(char *dst, const char *src)
{
  while (*src != '\0')
    *dst++ = *src++;

  return dst;
}

// Append check count code.
static void commChannelPrepareMessage(char *buffer, const char *message)
{
  char *pMsg;
  unsigned char mlen;

  pMsg = msgcpy(buffer, message);
  mlen = (uint8_t)(pMsg - buffer);
  pMsg += sprintf(pMsg, ":%02u", mlen);
}

static void commChannelRequestTransmit(CockpitNodeComm_t *pCommChannel)
{
  ioControlAddNodeWrite(pCommChannel, pCommChannel->commPortFd);
}

static void commChannelCancelTransmit(CockpitNodeComm_t *pCommChannel)
{
  ioControlRemoveNodeWrite(pCommChannel, pCommChannel->commPortFd);
}


static void commPortSendMessage(CockpitNodeComm_t *pCommChannel, const char *message)
{
  int msgLength = strlen(message);
  int result;

  strcpy(pCommChannel->lastMessageSent, message);
  pCommChannel->lastMessageSent[39] = '\0';

  if (logLevel > 1)
    cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: Send " << message << endl;

  do {
    result = write(pCommChannel->commPortFd, message, msgLength);
    if (result < 0)
    {
      if (logLevel > 1)
	cout << "Error: commChannel[" << pCommChannel->commPortPath << "]: write error(1): " << strerror(errno) << endl;
      pCommChannel->pLastAction = "<comm write err1>";
    }
    else
    {
      msgLength -= result;
      message += result;
    }
  } while ((result > 0) && (msgLength > 0));
  if (result >= 0)
  {
    result = write(pCommChannel->commPortFd, "\n", 1);
    if (result < 0)
    {
      if (logLevel > 1)
	cout << "Error: commChannel[" << pCommChannel->commPortPath << "]: write error(2): " << strerror(errno) << endl;
      pCommChannel->pLastAction = "<comm write err2>";
    }
  }
}

static void commChannelComposeNodeMessage(CockpitNodeComm_t *pCommChannel, char *buffer)
{
  char nodeMessage[64];

  pCommChannel->node->composeNodeMessage(nodeMessage);
  commChannelPrepareMessage(buffer, nodeMessage);
}

static void commChannelSendNodeMessage(CockpitNodeComm_t *pCommChannel)
{
  char transmitMessage[96];

  commChannelComposeNodeMessage(pCommChannel, transmitMessage);
  commPortSendMessage(pCommChannel, transmitMessage);
}

static void commChannelCloseNode(CockpitNodeComm_t *pCommChannel)
{
  if (pCommChannel->node != NULL)
  {
    if (logLevel > 1)
      cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: Closing " << pCommChannel->node->getCockpitNodeName() << endl;

    delete(pCommChannel->node);
    pCommChannel->node = NULL;
  }
}

static void commChannelClosePort(CockpitNodeComm_t *pCommChannel)
{
  if (pCommChannel->commPortFd > 0)
  {
    if (logLevel > 1)
      cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: Closing port." << endl;

    ioControlRemoveNode(pCommChannel, pCommChannel->commPortFd);
    close(pCommChannel->commPortFd);
    pCommChannel->commPortFd = -1;
  }
}

static void commChannelReset(CockpitNodeComm_t *pCommChannel)
{
  if (logLevel > 1)
    cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: Reset." << endl;

  commChannelCloseNode(pCommChannel);
  commChannelClosePort(pCommChannel);
  commChannelSetState(pCommChannel, chstate_OpenDelay);
  pCommChannel->delayTime = 1000;  // 1 second was 2.5 seconds
  pCommChannel->lastMessageReceived[0] = '@';
  pCommChannel->lastMessageReceived[1] = '\0';
  pCommChannel->lastMessageSent[0] = '#';
  pCommChannel->lastMessageSent[1] = '\0';
}

static void commChannelSendIdentifyRequest(CockpitNodeComm_t *pCommChannel)
{
  char transmitMessage[32];

  commChannelPrepareMessage(transmitMessage, "!IDENTIFY");
  commPortSendMessage(pCommChannel, transmitMessage);
  pCommChannel->delayTime = 1000;  // 1 seconds timeout on response from node
  pCommChannel->pLastAction = "<send identify>";
}

static void commChannelSendStatusRequest(CockpitNodeComm_t *pCommChannel)
{
  char transmitMessage[32];

  commChannelPrepareMessage(transmitMessage, "!STATUS");
  commPortSendMessage(pCommChannel, transmitMessage);
  pCommChannel->delayTime = 2000;  // 2 seconds
  pCommChannel->pLastAction = "<send status req>";
}

char isDecimalNumber(char ch)
{
  switch (ch)
  {
    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
      return !0;
    default:
      return 0;
  }
}

static char isMessageCodeValid(char *message)
{
  int charCount;
  int checkCount;
  char *pColon;
  char fOk;

  fOk = 0;

  // Count characters in the message.
  charCount = 0;
  pColon = message;
  while (*pColon != '\0')
  {
    charCount += 1;
    pColon += 1;
  }

  if (charCount > 0)
  {
    // Step backwards until ':' is found.
    do {
      pColon -= 1;
      charCount -= 1;
    } while ((charCount > 0) && (*pColon != ':'));

    if (*pColon == ':')
    {
      // Move past the ':'
      pColon += 1;

      // Parse the checkcount.
      checkCount = 0;
      while (isDecimalNumber(*pColon))
      {
	checkCount = checkCount * 10 + (*pColon - '0');
	pColon += 1;
      }

      fOk = (charCount == checkCount);
    }
  }
  return fOk;
}

static void commChannelWaitReq(CockpitNodeComm_t *pCommChannel);

static void initiateNodeMessageSend(CockpitNodeComm_t *pCommChannel)
{
  if (pCommChannel->retry > 0)
  {
    pCommChannel->retry -= 1;
    pCommChannel->delayTime = 500;
    commChannelRequestTransmit(pCommChannel);
  }
  else
  {
    char buffer[96];

    // Re-compose message so it can be logged.
    commChannelComposeNodeMessage(pCommChannel, buffer);
    if (logLevel > 1)
    {
      cout << "Error: Node[" << pCommChannel->node->getCockpitNodeName() << ":" << pCommChannel->commPortPath <<
	      "]: Message send failed: " << buffer << endl;
    }
    pCommChannel->pLastAction = "<msg send failure>";

    // Message transmission is just dropped... what else to do?  Reset the channel?
    commChannelWaitReq(pCommChannel);
  }
}

static void requestNodeMessageSend(CockpitNodeComm_t *pCommChannel)
{
  commChannelSetState(pCommChannel, chstate_WaitResponse);
  pCommChannel->retry = 20;
  initiateNodeMessageSend(pCommChannel);
}

static void commChannelNodeMessageRetry(CockpitNodeComm_t *pCommChannel)
{
  initiateNodeMessageSend(pCommChannel);
}

static void commChannelWaitReq(CockpitNodeComm_t *pCommChannel)
{
  commChannelSetState(pCommChannel, chstate_WaitReq);

  // Check if there is a deferred message waiting.
  if (pCommChannel->isNodeModified)
  {
    pCommChannel->isNodeModified = 0;
    requestNodeMessageSend(pCommChannel);
  }
  else
  {
    // if no app requests for a while then poll status.
    pCommChannel->delayTime = 10000;
  }
}

void cockpitNodeResponseAck(CockpitNodeComm_t *pCommChannel)
{
  pCommChannel->delayTime = 0;
  commChannelWaitReq(pCommChannel);
}

void cockpitNodeResponseNak(CockpitNodeComm_t *pCommChannel)
{
  pCommChannel->delayTime = 0;
  commChannelNodeMessageRetry(pCommChannel);
}

void cockpitNodeResponseErr(CockpitNodeComm_t *pCommChannel)
{
  if (logLevel > 1)
    cout << "Info: cockpitNodeResponseErr" << endl;
  pCommChannel->pLastAction = "<response error>";

  pCommChannel->delayTime = 0;
  commChannelReset(pCommChannel);
}

static void commChannelConnect(CockpitNodeComm_t *pCommChannel, CockpitNode *node)
{
  pCommChannel->node = node;
  commChannelWaitReq(pCommChannel);
}

static void commChannelProcessIdentifyResponse(CockpitNodeComm_t *pCommChannel, char *msg)
{
  char *pMessage = msg;

  if (fStringStartsWith(pMessage, "@IDENTIFY:"))
  {
    pMessage += 10;   // move past marker
    if (fStringStartsWith(pMessage, "ASI"))
      commChannelConnect(pCommChannel, createCockpitASI(pCommChannel));
    else if (fStringStartsWith(pMessage, "ATI"))
      commChannelConnect(pCommChannel, createCockpitATI(pCommChannel));
    else if (fStringStartsWith(pMessage, "BPI"))
      commChannelConnect(pCommChannel, createCockpitBrakePressureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "CAI"))
      commChannelConnect(pCommChannel, createCockpitCarbTemperatureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "CYI"))
      commChannelConnect(pCommChannel, createCockpitCylTemperatureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "FGI"))
      commChannelConnect(pCommChannel, createCockpitFlapsGearGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "FAT"))
      commChannelConnect(pCommChannel, createFreeAirTemperatureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "FPI"))
      commChannelConnect(pCommChannel, createCockpitFuelPressureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "FLF"))
      commChannelConnect(pCommChannel, createFrontTankFuelLevelGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "FLR"))
      commChannelConnect(pCommChannel, createRearTankFuelLevelGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "FLA"))
      commChannelConnect(pCommChannel, createAuxTankFuelLevelGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "HPI"))
      commChannelConnect(pCommChannel, createCockpitHydraulicPressureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "MPI"))
      commChannelConnect(pCommChannel, createCockpitMPI(pCommChannel));
    else if (fStringStartsWith(pMessage, "OPI"))
      commChannelConnect(pCommChannel, createCockpitOilPressureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "OTI"))
      commChannelConnect(pCommChannel, createCockpitOilTemperatureGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "RPM"))
      commChannelConnect(pCommChannel, createCockpitTachometer(pCommChannel));
    else if (fStringStartsWith(pMessage, "SUI"))
      commChannelConnect(pCommChannel, createCockpitSuctionGauge(pCommChannel));
    else if (fStringStartsWith(pMessage, "VSI"))
      commChannelConnect(pCommChannel, createCockpitVSI(pCommChannel));
    else if (fStringStartsWith(pMessage, "QDC"))
      commChannelConnect(pCommChannel, createCockpitQDC(pCommChannel));
    else
    {
      if (logLevel > 1)
	cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: Invalid identify: " << msg << endl;
      pCommChannel->pLastAction = "<invalid identify 1>";
    }

    // If the cockpit node was created then ask it to verify the identify message.
    if (pCommChannel->node != NULL)
      pCommChannel->node->processMessageResponse(msg);

    pCommChannel->pLastAction = "<identify>";
  }
  else
  {
    if (logLevel > 1)
      cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: Invalid response (not identify): " << msg << endl;
    pCommChannel->pLastAction = "<invalid identify 2>";
  }
}


static void commChannelProcessUnknownMessage(CockpitNodeComm_t *pCommChannel, char *buffer)
{
  if (logLevel > 1)
    cout << "Info: [" << pCommChannel->commPortPath << "]: Unkown message (dropped): " << buffer << endl;
  pCommChannel->pLastAction = "<unknown response>";
}


static void commChannelProcessInformational(CockpitNodeComm_t *pCommChannel, char *buffer)
{
  if (logLevel > 1)
  {
    cout << "Info: [";
    if (pCommChannel->node != NULL)
      cout << pCommChannel->node->getCockpitNodeName() << ":";
    cout << pCommChannel->commPortPath << "]: " << buffer << endl;
  }

  pCommChannel->pLastAction = "<informational>";
}

static void commChannelProcessResponse(CockpitNodeComm_t *pCommChannel, char *pMessage)
{
  uint8_t msgLength = strlen(pMessage);

  // Strip off line-feed if present.
  if (pMessage[msgLength - 1] == '\n')
  {
    pMessage[msgLength - 1] = 0;    
  }

  // Save message for status display
  strcpy(pCommChannel->lastMessageReceived, pMessage);

  // Decode and process message.
  if (pMessage[0] == '#')
  {
    commChannelProcessInformational(pCommChannel, pMessage);
  }
  else if (isMessageCodeValid(pMessage))
  {
    //    commChannelProcessInformational(pCommChannel, pMessage);

    if (pCommChannel->state == chstate_Identify)
      commChannelProcessIdentifyResponse(pCommChannel, pMessage);
    //    else if ((pCommChannel->state == chstate_WaitResponse) || (pCommChannel->state == chstate_ReqStatus))
    //      pCommChannel->node->processMessageResponse(pMessage);
    else if (pCommChannel->node != NULL)
      pCommChannel->node->processMessageResponse(pMessage);
    else
      commChannelProcessUnknownMessage(pCommChannel, pMessage);
  }
  else
  {
    pCommChannel->pLastAction = "<invalid message>";

    if (logLevel > 1)
    {
      cout << "Info: [";
      if (pCommChannel->node != NULL)
	cout << pCommChannel->node->getCockpitNodeName() << ":";
      cout << pCommChannel->commPortPath << "]: Invalid message format: " << pMessage << endl;
    }
  }
}


static void commChannelAttemptOpen(CockpitNodeComm_t *pCommChannel)
{
  assert(pCommChannel->delayTime == 0);
  assert(pCommChannel->state == chstate_OpenDelay);

  if (isCommChannelOpen(pCommChannel))
  {
    pCommChannel->isOpenErrorReported = 0;
    if (logLevel > 1)
      cout << "Info: CommChannel[" << pCommChannel->commPortPath <<
	      "]: Port open (fd: " << pCommChannel->commPortFd << ")" << endl;

    pCommChannel->pLastAction = "<port open>";

    ioControlAddNodeRead(pCommChannel, pCommChannel->commPortFd);
    pCommChannel->retry = 10;
    commChannelSetState(pCommChannel, chstate_Identify);
    commChannelRequestTransmit(pCommChannel);
  }
  else
  {
    // Channel state to remain "open delay".  Wait and try again later.
    pCommChannel->delayTime = 5000;  // 5 seconds
    if (pCommChannel->isOpenErrorReported == 0)
    {
      if (logLevel > 1)
	cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: comm port open failed." << endl;
      pCommChannel->isOpenErrorReported = 1;
    }
    pCommChannel->pLastAction = "<empty slot>";
  }
}

static void commChannelRequestTimeout(CockpitNodeComm_t *pCommChannel)
{
  if (pCommChannel->retry > 0)
  {
    pCommChannel->retry -= 1;
    commChannelRequestTransmit(pCommChannel);
  }
  else
  {
    pCommChannel->pLastAction = "<identify timeout>";

    // Identify request has failed.  Wait and try again later.
    commChannelReset(pCommChannel);
  }
}

static void commChannelRequestStatus(CockpitNodeComm_t *pCommChannel)
{
  pCommChannel->retry = 4;
  commChannelSetState(pCommChannel, chstate_ReqStatus);
  commChannelRequestTransmit(pCommChannel);
}

static void processCommChannelTimeout(CockpitNodeComm_t *pCommChannel)
{
  switch (pCommChannel->state)
  {
    case chstate_OpenDelay:
      commChannelAttemptOpen(pCommChannel);
      break;
    case chstate_Identify:
      commChannelRequestTimeout(pCommChannel);
      break;
    case chstate_WaitReq:
      commChannelRequestStatus(pCommChannel);
      break;
    case chstate_WaitResponse:
      commChannelNodeMessageRetry(pCommChannel);
      break;
    case chstate_ReqStatus:
      commChannelRequestTimeout(pCommChannel);
      break;
    default:
      if (logLevel > 1)
	cout << "Info: CommChannel[" << pCommChannel->commPortPath << "]: invalid state: " << pCommChannel->state << endl;
      break;
  }
}

static void processCommChannelTimeElapse(CockpitNodeComm_t *pCommChannel, unsigned long elapsedMilliseconds)
{
  if (pCommChannel->delayTime > 0)
  {
    if (elapsedMilliseconds < pCommChannel->delayTime)
    {
      pCommChannel->delayTime -= elapsedMilliseconds;
    }
    else
    {
      pCommChannel->delayTime = 0;
      processCommChannelTimeout(pCommChannel);
    }
  }
}

void processCockpitIndicators(void)
{
  unsigned long elapsedMilliseconds;
  int i;

  elapsedMilliseconds = GetMillisecondsElapsed();
  if (elapsedMilliseconds != 0)
  {
    for (i = 0; i < maxIndicatorTable; i++)
      processCommChannelTimeElapse(&cockpitChannel[i], elapsedMilliseconds);
  }
}


void getCockpitIndicatorStatus(int channel, char *str)
{
  CockpitNodeComm_t *pCommChannel;

  if (channel < maxIndicatorTable)
  {
    pCommChannel = &cockpitChannel[channel];

    sprintf(str, "%-15s %-14s %-20s %3s %-25s %-30s %-30s %-30s",
	    commChannelGetPortPath(pCommChannel),
	    getCockpitStateName(pCommChannel->state),
	    pCommChannel->pLastAction,
	    (pCommChannel->node == NULL) ? "***" : pCommChannel->node->getCockpitNodeCode(),
	    (pCommChannel->node == NULL) ? "<none>" : pCommChannel->node->getCockpitNodeName(),
	    (pCommChannel->node == NULL) ? "" : pCommChannel->node->getCockpitNodeInfo(),
	    pCommChannel->lastMessageSent,
	    pCommChannel->lastMessageReceived);
  }
  else
  {
    sprintf(str, "Invalid channel %d, max: %d", channel, maxIndicatorTable);
  }
}


static void initCockpitCommChannels(void)
{
  int i;

  for (i = 0; i < maxIndicatorTable; i++)
  {
    sprintf(cockpitChannel[i].commPortPath, "/dev/ttyUSB%d", i);
    cockpitChannel[i].commPortFd = -1;
    cockpitChannel[i].isOpenErrorReported = 1;
    cockpitChannel[i].node = NULL;
    cockpitChannel[i].isNodeModified = 0;
    cockpitChannel[i].pLastAction = "<init>";
    cockpitChannel[i].lastMessageSent[0] = '\0';
    cockpitChannel[i].lastMessageReceived[0] = '\0';
    commChannelReset(&cockpitChannel[i]);
  }
}

void indicateNodeModified(CockpitNodeComm_t *pCommChannel)
{
  if (pCommChannel->state == chstate_WaitReq)
  {
    requestNodeMessageSend(pCommChannel);
  }
  else
  {
    // Can't send a message at the moment, but remember that it is requested.
    if (logLevel > 1)
      cout << "Info: CommChannel[" << pCommChannel->node->getCockpitNodeName() << "]: message pending." << endl;

    pCommChannel->isNodeModified = 1;
  }
}


void processCockpitNodeWrite(CockpitNodeComm_t *pCommChannel)
{
  if (logLevel > 1)
    cout << "processCockpitNodeWrite(" << pCommChannel->commPortPath << "): " << getCockpitStateName(pCommChannel->state) << endl;

  if (pCommChannel->state == chstate_WaitResponse)
  {
    commChannelSendNodeMessage(pCommChannel);
  }
  else if (pCommChannel->state == chstate_ReqStatus)
  {
    commChannelSendStatusRequest(pCommChannel);
  }
  else if (pCommChannel->state == chstate_Identify)
  {
    commChannelSendIdentifyRequest(pCommChannel);
  }

  commChannelCancelTransmit(pCommChannel);
}

void processCockpitNodeRead(CockpitNodeComm_t *pCommChannel)
{
  int res;
  char buffer[256];

  // When a channel is reset, other events may still be pending for the channel.  Drop the
  // event if the channel is in the default/reset state.
  //
  if (pCommChannel->state != chstate_OpenDelay)
  {
    res = read(pCommChannel->commPortFd, buffer, 255);
    if (res < 0)
    {
      // Non-blocking I/O may yield EAGAIN.
      if (errno != EAGAIN)
      {
	if (logLevel > 1)
	  cout << "CommChannel[" << pCommChannel->commPortPath << "]: Read error(" << errno << "): " << strerror(errno) << endl;

	pCommChannel->pLastAction = "<comm read err1>";
      }
    }
    else if (res > 0)
    {
      buffer[res] = '\0';
      commChannelProcessResponse(pCommChannel, buffer);
    }
  }
}

void processCockpitNodeHup(CockpitNodeComm_t *pCommChannel)
{
  if (logLevel > 1)
  {
    cout << "Info: CommChannel[" << pCommChannel->commPortPath;
    if (pCommChannel->node != NULL)
      cout << ", " << pCommChannel->node->getCockpitNodeName();
    cout << "]: Comm port hang up." << endl;
  }

  pCommChannel->pLastAction = "<hang up>";
  commChannelReset(pCommChannel);
}

void initCockpitPanel(void)
{
  GetMillisecondsElapsed();   // fetch and throw away initial value.
  initCockpitCommChannels();
}

