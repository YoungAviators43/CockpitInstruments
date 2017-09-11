
#include "SerialUartSpy.h"

#define _BV(n)  (1 << (n))

static uint8_t UCSR0A = 0;
#define MPCM0   0
#define U2X0    1
#define UPE0    2
#define DOR0    3
#define FE0     4
#define UDRE0   5
#define TXC0    6
#define RXC0    7

static uint8_t UCSR0B = 0;
#define TXB80   0
#define RXB80   1
#define UCSZ02  2
#define TXEN0   3
#define RXEN0   4
#define UDRIE0  5
#define TXCIE0  6
#define RXCIE0  7

static uint8_t UCSR0C;

static uint16_t UBRR0 = 0;

static uint8_t UDR0 = 0xFF;

#define ISR(vect)  void (vect)(void)

ISR(USART_TX_vect);
ISR(USART_UDRE_vect);
ISR(USART_RX_vect);

flag SerialUartSpy_IsUartCreated(void)
{
  return ((UCSR0B & (_BV(TXEN0) | _BV(RXEN0))) != 0);
}

uint32_t SerialUartSpy_GetBaudRate(void)
{
  return (uint32_t)(F_CLK_IO / 16UL / (uint32_t)(UBRR0 + 1));
}

char SerialUartSpy_GetLastTxChar(void)
{
  return (char)UDR0;
}

void SerialUartSpy_RunTxbInterrupt(void)
{
  if (SerialUartSpy_IsTxbInterruptEnabled())
  {
    UCSR0A |= _BV(UDRE0);
    USART_UDRE_vect();
    UCSR0A &= (uint8_t)~_BV(UDRE0);
  }
}


void SerialUartSpy_RunRxbInterrupt(char ch)
{
  if (SerialUartSpy_IsRxbInterruptEnabled())
  {
    UCSR0A &= (uint8_t)~(_BV(DOR0) | _BV(UPE0) | _BV(FE0));
    UCSR0A |= _BV(UDRE0);
    UDR0 = (uint8_t)ch;
    USART_RX_vect();
  }
}

void SerialUartSpy_RunRxbInterruptFrameError(char ch)
{
  if (SerialUartSpy_IsRxbInterruptEnabled())
  {
    UCSR0A &= (uint8_t)~(_BV(DOR0) | _BV(UPE0) | _BV(FE0));
    UCSR0A |= _BV(UDRE0) | _BV(FE0);
    UDR0 = (uint8_t)ch;
    USART_RX_vect();
  }
}

void SerialUartSpy_RunRxbInterruptOverrunError(char ch)
{
  if (SerialUartSpy_IsRxbInterruptEnabled())
  {
    UCSR0A &= (uint8_t)~(_BV(DOR0) | _BV(UPE0) | _BV(FE0));
    UCSR0A |= _BV(UDRE0) | _BV(DOR0);
    UDR0 = (uint8_t)ch;
    USART_RX_vect();
  }
}

flag SerialUartSpy_IsTxbInterruptEnabled(void)
{
  return (UCSR0B & _BV(UDRIE0)) != 0;
}

flag SerialUartSpy_IsRxbInterruptEnabled(void)
{
  return (UCSR0B & _BV(RXCIE0)) != 0;
}

///////////////////////////////////////////////////

#define AVR_HAL
#include "SerialUart.c"

///////////////////////////////////////////////////
