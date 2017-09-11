
#include "CommonDefs.h"
#include "SerialMessage.h"
#include "MessageProcessSpy.h"

static MpsRequest_t lastCommandDecoded;

///typedef void (*CommandProc_t)(const char *message);
void mpsIdentifyReq(const char *message)
{
  message = message;
  lastCommandDecoded = MPS_IDENTIFY_REQ;
}

void mpsStatusReq(const char *message)
{
  message = message;
  lastCommandDecoded = MPS_STATUS_REQ;
}

void mpsUnknownReq(const char *message)
{
  message = message;
  lastCommandDecoded = MPS_UNKNOWN_REQ;
}

static CommandRecord_t TestCommandTable[] =
  { { "!IDENTIFY?", mpsIdentifyReq },
    { "!STATUS?",   mpsStatusReq   },
    { NULL,         mpsUnknownReq  }
  };

void MessageProcessSpy_Decode(void)
{
  lastCommandDecoded = MPS_NONE;
  SerialMessage_Process(TestCommandTable);
}

MpsRequest_t MessageProcessSpy_LastCommand(void)
{
  return lastCommandDecoded;
}
