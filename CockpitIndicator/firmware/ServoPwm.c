////////////////////////////////////////////////////////////////
///
/// Servo PWM implementation
///
/// Contains all the magic numbers for converting a servo position
/// from xxx.xx percent (as a scaled integer) into a pwm value
/// and loading into the timer/counter.
///
////////////////////////////////////////////////////////////////

#ifdef AVR_HAL

#include "CommonDefs.h"
#include "ServoPwm.h"

#define SERVO_PWM_RATE   50       // 50 hz, 20ms
//#define SERVO_PWM_LOWER  2304U     // pulse of 1000us duration
#define SERVO_PWM_LOWER  1152U     // pulse of  500us duration
#define SERVO_PWM_MID    3456U     // pulse of 1500us duration
//#define SERVO_PWM_UPPER  4608U     // pulse of 2000us duration
#define SERVO_PWM_UPPER  5760U     // pulse of 2500us duration
#define SERVO_PWM_RANGE  (SERVO_PWM_UPPER - SERVO_PWM_LOWER)

static void ServoPwm_LoadChannelA(uint16_t pwmValue)
{
  OCR1A = pwmValue;
}

static void ServoPwm_LoadChannelB(uint16_t pwmValue)
{
  OCR1B = pwmValue;
}

uint16_t calcPwmOffset(uint32_t newPosition)
{
  return (uint16_t)((newPosition * (uint32_t)SERVO_PWM_RANGE) / (uint32_t)10000U);
}

static uint16_t calcPwmValue(ServoPosition_t newPosition)
{
  return (uint16_t)(calcPwmOffset(newPosition) + SERVO_PWM_LOWER);
}

void ServoPwm_Create(void)
{
  ICR1 = ((F_CLK_IO / 8UL) / SERVO_PWM_RATE) - 1;
  ServoPwm_LoadChannelA(calcPwmValue(5000));
  ServoPwm_LoadChannelB(calcPwmValue(5000));
  TCCR1A = 0xA2;  // clear on match, set at BOTTOM, fast pwm, use ICR1 for TOP.
  TCCR1B = 0x1A;  // fast pwm, f_io_clk / 8  (starts timer)
  DDRB = _BV(DDB1)|_BV(DDB2);  // set pin to output of OC1A and OC1B
}

/// Position units are: xxx.xx% as a scaled integer: 10000 => 100%
void ServoPwm_SetServoPosition(ServoPwmId_t channelId, ServoPosition_t newPosition)
{
  if (channelId == ServoPwm_A)
  {
    ServoPwm_LoadChannelA(calcPwmValue(newPosition));
  }
  else if (channelId == ServoPwm_B)
    ServoPwm_LoadChannelB(calcPwmValue(newPosition));
}

#endif
