/// Cockpit Node Controller firmware
///
/// SerialUart implementation.

#ifdef AVR_HAL

#include "CommonDefs.h"
#include "SerialUart.h"

void SerialUart_Create(void)
{
  UCSR0A = 0;
  UCSR0B = _BV(TXEN0) | _BV(RXEN0);
  UCSR0C = 0x06;   // asynchronous, 8n1 mode

  SerialUart_DisableRxb();
  SerialUart_DisableTxb();
}

void SerialUart_Initialize(void)
{
  UBRR0 = (uint16_t)(((uint32_t)(F_CLK_IO / 16UL) / COMMUNICATION_BAUD_RATE) - 1);
}

////////////////////////////////////////////////////////

static void (*TxbCallback)(void);

void SerialUart_EnableTxb(void (*TxbCallbackFunction)(void))
{
  TxbCallback = TxbCallbackFunction;
  UCSR0B |= _BV(UDRIE0);
}

void SerialUart_DisableTxb(void)
{
  UCSR0B &= (uint8_t)~_BV(UDRIE0);
  TxbCallback = NULL;
}

ISR(USART_UDRE_vect)
{
  if (TxbCallback != NULL)
    TxbCallback();
}

void SerialUart_TxChar(char ch)
{
  UDR0 = (uint8_t)ch;
}

////////////////////////////////////////////////////////

static void (*RxbCallback)(void);

void SerialUart_EnableRxb(void (*RxbCallbackFunction)(void))
{
  RxbCallback = RxbCallbackFunction;
  UCSR0B |= _BV(RXCIE0);
}

void SerialUart_DisableRxb(void)
{
  UCSR0B &= (uint8_t)~_BV(RXCIE0);
  RxbCallback = NULL;
}

ISR(USART_RX_vect)
{
  if (RxbCallback != NULL)
    RxbCallback();
}

flag SerialUart_fRxGetChar(char *pCh)
{
  char rxChar;
  uint8_t rxStatus;

  rxStatus = UCSR0A;
  if ((rxStatus & _BV(UDRE0)) != 0)
  {
    rxChar = (char)UDR0;
    if ((rxStatus & (_BV(FE0) | _BV(DOR0) | _BV(UPE0))) == 0)
    {
      *pCh = rxChar;
      return TRUE;
    }
  }
  return FALSE;
}

#endif
/// End of File
