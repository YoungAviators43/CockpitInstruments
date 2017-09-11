/// Cockpit Node Controller firmware
///
/// Timer Service implementation.

#ifdef AVR_HAL

#include "CommonDefs.h"
#include "TimerService.h"

static TimerServiceRoutine_t TimerServiceRoutine;

void TimerService_Create(void)
{
  // Disable all timer 2 interrupts.
  TIMSK2 = 0;
  TimerServiceRoutine = NULL;

  // Reset timer 2 to default mode.
  TCCR2A = 0x00;
  TCCR2B = 0x00;
}

void TimerService_Destroy(void)
{
}

void TimerService_Activate(uint8_t rate, TimerServiceRoutine_t routine)
{
  OCR2A = (uint8_t)(((unsigned int)(F_CLK_IO / 1024UL) / (unsigned int)rate) - 1);

  // CTC mode, Source: clk / 1024
  TCCR2A = 0x02;
  TCCR2B = 0x07;

  // Register interrupt processing routine and enable rollover interrupt.
  TimerServiceRoutine = routine;
  TIMSK2 |= _BV(OCIE2A);
}

ISR(TIMER2_COMPA_vect)
{
  if (TimerServiceRoutine != NULL)
    TimerServiceRoutine();
}

#endif
