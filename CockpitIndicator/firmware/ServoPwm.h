/// Cockpit Node Controller firmware

#ifndef SERVOPWM_H
#define SERVOPWM_H

/// Servo PWM interface.

#include "CommonDefs.h"

typedef uint16_t ServoPosition_t;
typedef enum { ServoPwm_None, ServoPwm_A, ServoPwm_B } ServoPwmId_t;

void ServoPwm_Create(void);
void ServoPwm_SetServoPosition(ServoPwmId_t channelId, ServoPosition_t newPosition);

#endif
/// End of File
