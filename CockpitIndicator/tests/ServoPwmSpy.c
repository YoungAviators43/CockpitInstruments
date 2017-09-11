/// Servo PWM spy implementation.

#include "CommonDefs.h"
#include "ServoPwmSpy.h"

#define _BV(n)  (1 << (n))

static uint8_t TCCR1A;  // timer/counter 1 control register A
static uint8_t TCCR1B;  // timer/counter 1 control register B
static uint16_t ICR1;   // timer/counter 1 input capture register
static uint16_t OCR1A;  // timer/counter 1 output compare register A
static uint16_t OCR1B;  // timer/counter 1 output compare register B

static uint8_t DDRB;
#define DDB0 0
#define DDB1 1
#define DDB2 2
#define DDB3 3
#define DDB4 4
#define DDB5 5
#define DDB6 6
#define DDB7 7

flag ServoPwmSpy_OkInit(void)
{
  return
    (TCCR1A == 0xA2) &&
    (TCCR1B == 0x1A) &&
    (ICR1 == 46079) &&
    (OCR1A == 3456) &&
    (OCR1B == 3456) &&
    (DDRB == 0x06);
}

uint16_t ServoPwmSpy_GetPwmValueA(void)
{
  return OCR1A;
}

#define AVR_HAL
#include "ServoPwm.c"

/// End of File
