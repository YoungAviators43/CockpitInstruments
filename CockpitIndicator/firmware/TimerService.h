/// Cockpit Node Controller firmware
///
/// Timer service interface.

#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include "CommonDefs.h"

typedef void (*TimerServiceRoutine_t)(void);

void TimerService_Create(void);
void TimerService_Destroy(void);

void TimerService_Activate(uint8_t rate, TimerServiceRoutine_t routine);

#endif
