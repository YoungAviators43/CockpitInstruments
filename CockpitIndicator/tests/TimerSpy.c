
#include "TimerSpy.h"

#define _BV(n)  (1 << (n))

static uint8_t TIMSK2;
#define TOIE2 0
#define OCIE2A 1
#define OCIE2B 2

static int8_t TIFR2;
#define TOV2 0
#define OCF2A 1
#define OCF2B 2

static uint8_t TCCR2A;
#define WGM20 0
#define WGM21 1
#define COM2B0 4
#define COM2B1 5
#define COM2A0 6
#define COM2A1 7

static uint8_t TCCR2B;
#define CS20 0
#define CS21 1
#define CS22 2
#define WGM22 3
#define FOC2B 6
#define FOC2A 7

#define ISR(vect)  void (vect)(void)

ISR(TIMER2_COMPA_vect);

static uint8_t OCR2A;

static int serviceRoutineCalls;

void TimerServiceSpy_Reset(void)
{
  TIMSK2 = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = 0;
  serviceRoutineCalls = 0;
}

flag TimerServiceSpy_IsTimerCreated(void)
{
  return TRUE;
}

flag TimerServiceSpy_IsTimerActive(void)
{
  return TimerServiceSpy_IsTimerInterruptEnabled();
}


flag TimerServiceSpy_IsTimerInterruptEnabled(void)
{
  return ((TIMSK2 & _BV(OCIE2A)) != 0);
}

void TimerServiceSpy_service(void)
{
  serviceRoutineCalls += 1;
}

flag TimerServiceSpy_IsCTCMode(void)
{
  return ((TCCR2A == 0x02) && (TCCR2B == 0x07));
}

flag TimerServiceSpy_IsRate(uint16_t rate)
{
  return ((unsigned int)(F_CLK_IO / 1024UL) / (unsigned int)(OCR2A + 1)) == rate;
}

void TimerServiceSpy_TimerRollover(int numTimes)
{
  while (numTimes-- > 0)
  {
    TIFR2 |= _BV(OCF2A);
    if (TimerServiceSpy_IsTimerInterruptEnabled())
    {
      TIMER2_COMPA_vect();
      TIFR2 &= ~_BV(OCF2A);
    }
  }
}

int TimerServiceSpy_TimerProcessCalled(void)
{
  return serviceRoutineCalls;
}

#define AVR_HAL

#include "TimerService.c"
