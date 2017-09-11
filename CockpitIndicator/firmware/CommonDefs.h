/// Cockpit Node Controller firmware
///
/// Common definitions.

#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include <stdint.h>

#define FALSE    0
#define TRUE     (!FALSE)

typedef char flag;

#ifndef NULL
#define NULL   ((void *)0)
#endif

#define F_CLK_IO  (18432000UL)

//#define COMMUNICATION_BAUD_RATE  (19200UL)
#define COMMUNICATION_BAUD_RATE  (38400UL)

#define PD2_LED1_ENABLE     2    // port D, bit 2
#define PD2_LED2_ENABLE     3    // port D, bit 3
#define PD2_LED3_ENABLE     4    // port D, bit 4

#define TICK_RATE  100
#define MS_PER_TICK (1000 / TICK_RATE)

#define TICKS(ms)  (((ms) + MS_PER_TICK - 1) / MS_PER_TICK)

void Led_TransmitOn(void);
void Led_TransmitOff(void);
void Led_ReceiveOn(void);
void Led_ReceiveOff(void);

#endif

/// End of File

