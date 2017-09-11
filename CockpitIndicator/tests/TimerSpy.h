
#include "TimerService.h"

void TimerServiceSpy_Reset(void);
flag TimerServiceSpy_IsTimerCreated(void);
flag TimerServiceSpy_IsTimerActive(void);
void TimerServiceSpy_service(void);
flag TimerServiceSpy_IsCTCMode(void);
flag TimerServiceSpy_IsRate(uint16_t rate);
flag TimerServiceSpy_IsTimerInterruptEnabled(void);
void TimerServiceSpy_TimerRollover(int numTimes);
int TimerServiceSpy_TimerProcessCalled(void);
