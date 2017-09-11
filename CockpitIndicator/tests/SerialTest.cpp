/// CockpitNode controller firmware test suite.
///
/// Test the serial message framing.

#include "CppUTest/TestHarness.h"

extern "C"
{
#include "CommonDefs.h"
#include "SerialUartSpy.h"
#include "SerialFrame.h"
#include "SerialMessage.h"
#include "MessageProcessSpy.h"
}

TEST_GROUP(SerialFrame)
{
  void setup(void)
  {
    SerialFrame_Create();
  }

  void teardown(void)
  {
     SerialFrame_Destroy();
  }

  void CheckRxMessageComplete(void)
  {
    CHECK(SerialFrame_IsRxMessageComplete());
    CHECK(!SerialUartSpy_IsRxbInterruptEnabled());
    CHECK(SerialFrame_IsRxIdle());
  }
};

/// Creation of SerialFrame module creates SerialUart module.
TEST(SerialFrame, Create)
{
  CHECK(SerialUartSpy_IsUartCreated());
  CHECK(!SerialUartSpy_IsTxbInterruptEnabled());
  CHECK(!SerialUartSpy_IsRxbInterruptEnabled());
}

/// Tx machine idle after create.
TEST(SerialFrame, TxIdle)
{
  CHECK(SerialFrame_IsTxIdle());
}

/// SerialFrame initialization sets uart baud rate.
TEST(SerialFrame, UartInit)
{
  CHECK_EQUAL(115200UL, SerialUartSpy_GetBaudRate());
  SerialFrame_Initialize();
  UNSIGNED_LONGS_EQUAL(115200UL, SerialUartSpy_GetBaudRate());
  CHECK(!SerialUartSpy_IsTxbInterruptEnabled());
  CHECK(!SerialUartSpy_IsRxbInterruptEnabled());
}

TEST(SerialFrame, TxMinimumFrame)
{
  SerialFrame_Initialize();
  CHECK(SerialFrame_IsTxIdle());
  SerialFrame_Transmit("");   // zero length, null message
  CHECK(!SerialFrame_IsTxIdle());
  CHECK(SerialUartSpy_IsTxbInterruptEnabled());
  SerialUartSpy_RunTxbInterrupt();
  CHECK(!SerialUartSpy_IsTxbInterruptEnabled());
  CHECK_EQUAL((int)'\n', (int)SerialUartSpy_GetLastTxChar());
  CHECK(SerialFrame_IsTxIdle());
}

TEST(SerialFrame, TxSmallFrame)
{
  SerialFrame_Initialize();
  CHECK(SerialFrame_IsTxIdle());
  SerialFrame_Transmit("0123");   // Short message
  CHECK(!SerialFrame_IsTxIdle());
  CHECK(SerialUartSpy_IsTxbInterruptEnabled());
  SerialUartSpy_RunTxbInterrupt();
  CHECK_EQUAL('0', SerialUartSpy_GetLastTxChar());
  CHECK(SerialUartSpy_IsTxbInterruptEnabled());
  SerialUartSpy_RunTxbInterrupt();
  CHECK_EQUAL('1', SerialUartSpy_GetLastTxChar());
  CHECK(SerialUartSpy_IsTxbInterruptEnabled());
  SerialUartSpy_RunTxbInterrupt();
  CHECK_EQUAL('2', SerialUartSpy_GetLastTxChar());
  CHECK(SerialUartSpy_IsTxbInterruptEnabled());
  SerialUartSpy_RunTxbInterrupt();
  CHECK_EQUAL('3', SerialUartSpy_GetLastTxChar());
  CHECK(SerialUartSpy_IsTxbInterruptEnabled());
  SerialUartSpy_RunTxbInterrupt();
  CHECK_EQUAL((int)'\n', (int)SerialUartSpy_GetLastTxChar());
  CHECK(!SerialUartSpy_IsTxbInterruptEnabled());
  CHECK(SerialFrame_IsTxIdle());
}


/// Rx machine idle after create.
TEST(SerialFrame, RxIdle)
{
  CHECK(SerialFrame_IsRxIdle());
}


TEST(SerialFrame, RxReadyToReceive)
{
  char buffer[32];

  SerialFrame_Initialize();
  CHECK(!SerialFrame_IsRxMessageComplete());
  SerialFrame_Receive(buffer, 32);
  CHECK(!SerialFrame_IsRxIdle());
  CHECK(!SerialFrame_IsRxMessageComplete());
  CHECK(SerialUartSpy_IsRxbInterruptEnabled());
}

TEST(SerialFrame, RxNullMessage)
{
  char buffer[32];

  SerialFrame_Initialize();
  SerialFrame_Receive(buffer, 32);
  CHECK(!SerialFrame_IsRxMessageComplete());
  CHECK(SerialUartSpy_IsRxbInterruptEnabled());
  SerialUartSpy_RunRxbInterrupt('\n');
  CheckRxMessageComplete();
  CHECK_EQUAL(0, (int)buffer[0]);
}


TEST(SerialFrame, RxShortMessage)
{
  char buffer[32];

  SerialFrame_Initialize();
  SerialFrame_Receive(buffer, 32);
  SerialUartSpy_RunRxbInterrupt('4');
  SerialUartSpy_RunRxbInterrupt('5');
  SerialUartSpy_RunRxbInterrupt('6');
  SerialUartSpy_RunRxbInterrupt('7');
  SerialUartSpy_RunRxbInterrupt('\n');
  CheckRxMessageComplete();
  STRCMP_EQUAL("4567", buffer);
}

TEST(SerialFrame, RxOverrun)
{
  char buffer[32];

  SerialFrame_Initialize();
  SerialFrame_Receive(buffer, 4);
  SerialUartSpy_RunRxbInterrupt('4');
  SerialUartSpy_RunRxbInterrupt('5');
  SerialUartSpy_RunRxbInterrupt('6');
  CHECK(!SerialFrame_IsRxMessageComplete());
  SerialUartSpy_RunRxbInterrupt('7');
  CHECK(!SerialFrame_IsRxMessageComplete());
  SerialUartSpy_RunRxbInterrupt('\n');
  CheckRxMessageComplete();
  STRCMP_EQUAL("456", buffer);
}

TEST(SerialFrame, RxMessageCompleteReset)
{
  char buffer[32];

  SerialFrame_Initialize();
  SerialFrame_Receive(buffer, 8);
  SerialUartSpy_RunRxbInterrupt('4');
  SerialUartSpy_RunRxbInterrupt('5');
  SerialUartSpy_RunRxbInterrupt('6');
  SerialUartSpy_RunRxbInterrupt('7');
  SerialUartSpy_RunRxbInterrupt('\n');
  CheckRxMessageComplete();
  STRCMP_EQUAL("4567", buffer);

  // Start a new receive request, RxMessageComplete should be false.
  SerialFrame_Receive(buffer, 8);
  CHECK(!SerialFrame_IsRxMessageComplete());
}

TEST(SerialFrame, RxUartFrameError)
{
  char buffer[32];

  SerialFrame_Initialize();
  SerialFrame_Receive(buffer, 32);
  SerialUartSpy_RunRxbInterrupt('4');
  SerialUartSpy_RunRxbInterrupt('5');
  SerialUartSpy_RunRxbInterruptFrameError('X');
  SerialUartSpy_RunRxbInterrupt('6');
  SerialUartSpy_RunRxbInterrupt('7');
  SerialUartSpy_RunRxbInterrupt('\n');
  CheckRxMessageComplete();
  STRCMP_EQUAL("4567", buffer);
}

TEST(SerialFrame, RxUartOverrunError)
{
  char buffer[32];

  SerialFrame_Initialize();
  SerialFrame_Receive(buffer, 32);
  SerialUartSpy_RunRxbInterrupt('A');
  SerialUartSpy_RunRxbInterrupt('B');
  SerialUartSpy_RunRxbInterruptOverrunError('X');
  SerialUartSpy_RunRxbInterrupt('C');
  SerialUartSpy_RunRxbInterrupt('D');
  SerialUartSpy_RunRxbInterrupt('\n');
  CheckRxMessageComplete();
  STRCMP_EQUAL("ABCD", buffer);
}

TEST_GROUP(SerialMessage)
{
  void setup(void)
  {
    SerialMessage_Create();
  }

  void teardown(void)
  {
     SerialMessage_Destroy();
  }

  void TestReceiveMessage(const char *message)
  {
    while (*message != '\0')
      SerialUartSpy_RunRxbInterrupt(*message++);
    SerialUartSpy_RunRxbInterrupt('\n');
  }

  void TestSendMessage(const char *message)
  {
    SerialUartSpy_RunTxbInterrupt();

    while (*message != '\0')
    {
      CHECK_EQUAL(*message, SerialUartSpy_GetLastTxChar());
      SerialUartSpy_RunTxbInterrupt();
      message += 1;
    }
    CHECK_EQUAL((int)'\n', (int)SerialUartSpy_GetLastTxChar());
  }
};

TEST(SerialMessage, Create)
{
  CHECK(SerialUartSpy_IsUartCreated());
  CHECK(!SerialUartSpy_IsTxbInterruptEnabled());
  CHECK(!SerialUartSpy_IsRxbInterruptEnabled());
  CHECK(SerialUartSpy_GetBaudRate() == COMMUNICATION_BAUD_RATE);
}

TEST(SerialMessage, RxNullMessage)
{
  SerialMessage_ReceiveEnable();
  CHECK(!SerialFrame_IsRxMessageComplete());
  TestReceiveMessage("");
  CHECK(SerialFrame_IsRxMessageComplete());
  CHECK(!SerialMessage_IsValid());
}

TEST(SerialMessage, RxTooShortMessage)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("ABCD");
  CHECK(SerialFrame_IsRxMessageComplete());
  CHECK(!SerialMessage_IsValid());
}

TEST(SerialMessage, RxShortMessage)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!:1");
  CHECK(SerialFrame_IsRxMessageComplete());
  CHECK(SerialMessage_IsValid());
}

TEST(SerialMessage, RxInvalidCount)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!:2");
  CHECK(SerialFrame_IsRxMessageComplete());
  CHECK(!SerialMessage_IsValid());
}

TEST(SerialMessage, RxIdentifyMessage)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!IDENTIFY?:10");
  CHECK(SerialFrame_IsRxMessageComplete());
  CHECK(SerialMessage_IsValid());
}

TEST(SerialMessage, TxNullMessage)
{
  SerialMessage_Transmit("");
  CHECK(!SerialFrame_IsTxIdle());
  TestSendMessage(":00");
  CHECK(SerialFrame_IsTxIdle());
}

TEST(SerialMessage, TxShortMessage)
{
  SerialMessage_Transmit("!ABC");
  CHECK(!SerialFrame_IsTxIdle());
  TestSendMessage("!ABC:04");
  CHECK(SerialFrame_IsTxIdle());
}

TEST(SerialMessage, TxIdentifyResponse)
{
  SerialMessage_Transmit("!ID:OTI V1.20 S/N:2002009");
  CHECK(!SerialFrame_IsTxIdle());
  TestSendMessage("!ID:OTI V1.20 S/N:2002009:25");
  CHECK(SerialFrame_IsTxIdle());
}

TEST(SerialMessage, RxUnknownDecode)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!STAND_ON_YOUR_HEAD:19");
  MessageProcessSpy_Decode();
  CHECK(MessageProcessSpy_LastCommand() == MPS_UNKNOWN_REQ);
}

TEST(SerialMessage, RxDecodeStatus)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!STATUS?:8");
  MessageProcessSpy_Decode();
  CHECK(MessageProcessSpy_LastCommand() == MPS_STATUS_REQ);
}

TEST(SerialMessage, RxIdentifyDecode)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!IDENTIFY?:10");
  MessageProcessSpy_Decode();
  CHECK(MessageProcessSpy_LastCommand() == MPS_IDENTIFY_REQ);
}

TEST(SerialMessage, RxIdentifyDecodeBadCount)
{
  SerialMessage_ReceiveEnable();
  TestReceiveMessage("!IDENTIFY?:11");
  MessageProcessSpy_Decode();
  CHECK(MessageProcessSpy_LastCommand() == MPS_NONE);
}

/// End of File
