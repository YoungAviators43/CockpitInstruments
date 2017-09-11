/// Cockpit Node Controller firmware
///
/// SerialFrame implementation.

#include "CommonDefs.h"
#include "SerialUart.h"
#include "SerialFrame.h"

static volatile const char *TxMessage;
static volatile flag RxMessageComplete;
static char *RxMessage;
static int RxIndex;
static uint8_t RxBufferSize;

void SerialFrame_Create(void)
{
  SerialUart_Create();
  TxMessage = NULL;
  RxMessage = NULL;
  RxMessageComplete = FALSE;
}

void SerialFrame_Destroy(void)
{
}

flag SerialFrame_IsRxIdle(void)
{
  return (RxMessage == NULL);
}

flag SerialFrame_IsTxIdle(void)
{
  return (TxMessage == NULL);
}

void SerialFrame_Initialize(void)
{
  SerialUart_Initialize();
  Led_TransmitOff();
  Led_ReceiveOff();
}

/// !! Runs in interrupt context !!
static void SerialFrame_TxBufferEmpty(void)
{
  if (*TxMessage != '\0')
  {
    // Transmit the next character in the message.
    SerialUart_TxChar(*TxMessage);
    TxMessage += 1;
  }
  else
  {
    // End of message, transmit End-of-Frame.
    SerialUart_DisableTxb();
    SerialUart_TxChar('\n');
    TxMessage = NULL;
    Led_TransmitOff();
  }
}

flag SerialFrame_TransmitReady(void)
{
  return (TxMessage == NULL);
}

void SerialFrame_Transmit(const char *message)
{
  if (message != NULL)
  {
#if 0
    while (TxMessage != 0)
    {
      // Null body.  Wait for previous message to complete.
    }
#endif
    // If message is already being transmitted, then drop this one.
    if (!SerialFrame_TransmitReady())
      return;

    TxMessage = message;
    Led_TransmitOn();
    SerialUart_EnableTxb(SerialFrame_TxBufferEmpty);
  }
}

flag SerialFrame_IsRxMessageComplete(void)
{
  return RxMessageComplete;
}

/// !! Runs in interrupt context !!
static void SerialFrame_RxCharReceived(void)
{
  char ch;

  if (SerialUart_fRxGetChar(&ch))
  {
    if (ch == '\n')
    {
      RxMessage[RxIndex] = '\0';
      SerialUart_DisableRxb();
      RxMessage = NULL;
      RxMessageComplete = TRUE;
    }
    else if (RxIndex < RxBufferSize - 1)
    {
      Led_ReceiveOn();
      RxMessage[RxIndex] = ch;
      RxIndex += 1;
    }
  }
}

void SerialFrame_Receive(char *buffer, uint8_t bufferSize)
{
  RxMessage = buffer;
  RxBufferSize = bufferSize;

  RxMessageComplete = FALSE;
  RxIndex = 0;
  SerialUart_EnableRxb(SerialFrame_RxCharReceived);
  Led_ReceiveOff();
}

/// End of File
