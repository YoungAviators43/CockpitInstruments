/// Cockpit Node Controller firmware
///
/// Message Handler implementation.

#include <string.h>
#include <stdio.h>

#include "CommonDefs.h"
#include "SerialFrame.h"
#include "SerialMessage.h"
#include "MessageParse.h"

void SerialMessage_Create(void)
{
  SerialFrame_Create();
  SerialFrame_Initialize();
}

void SerialMessage_Destroy(void)
{
  SerialFrame_Destroy();
}

static char receiveBuffer[96];

void SerialMessage_ReceiveEnable(void)
{
  SerialFrame_Receive(receiveBuffer, sizeof(receiveBuffer));
}

/// For testing only?
flag SerialMessage_IsValid(void)
{
  return SerialFrame_IsRxMessageComplete() && fMessageCodeValid(receiveBuffer);
}

void SerialMessage_Decode(char *pMessage, CommandRecord_t *CommandTable)
{
  while ((CommandTable->commandName != NULL) && !fStringStartsWith(pMessage, CommandTable->commandName))
  {
    CommandTable += 1;
  }

  if (CommandTable->commandProcessor != NULL)
  {
    CommandTable->commandProcessor(pMessage);
  }
}

void SerialMessage_Process(CommandRecord_t *CommandTable)
{
  if (SerialFrame_IsRxMessageComplete())
  {
    if (fMessageCodeValid(receiveBuffer))
    {
      SerialMessage_Decode(receiveBuffer, CommandTable);
    }
    else
    {
      SerialMessage_Transmit("@NAK:"/*node id*/ ": Invalid check code.");
    }
    // The message was: valid AND processed, OR not valid.
    // Ready to receive the next message.
    SerialMessage_ReceiveEnable();
  }
}

static char *msgcpy(char *dst, const char *src)
{
  while (*src != '\0')
    *dst++ = *src++;

  return dst;
}

// Append check count code.
void SerialMessage_PrepareMessage(char *buffer, const char *message)
{
  char *pMsg;
  uint8_t mlen;

  pMsg = msgcpy(buffer, message);
  mlen = (uint8_t)(pMsg - buffer);
  pMsg += sprintf(pMsg, ":%02u", mlen);
}

/// Prepend the node address and append the character check count.
void SerialMessage_Transmit(const char *message)
{
  static char transmitMessage[64];

  if (!SerialFrame_TransmitReady())
    return;

  SerialMessage_PrepareMessage(transmitMessage, message);
  SerialFrame_Transmit(transmitMessage);
}

// End of Frame
