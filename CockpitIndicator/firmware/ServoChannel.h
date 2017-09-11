/// Cockpit Node Controller firmware

/// Servo Channel interface.

#include "CommonDefs.h"
#include "ServoPwm.h"

typedef struct _servo_channel_ ServoChannel_t;

typedef enum { SERVO_CHANNEL_0, SERVO_CHANNEL_1, NUM_SERVO_CHANNELS } ServoChannelId_t;
typedef enum { SERVO_MODE_INACTIVE, SERVO_MODE_ACTIVE } ServoChannelMode_t;

typedef int16_t ServoDomain_t;
typedef uint16_t ServoRange_t;

typedef struct {
  ServoDomain_t domain;
  ServoRange_t servo;
} ServoTranslationRecord_t;

typedef struct {
  ServoPwmId_t servoPwmId;
  ServoDomain_t domainLowerBound;
  ServoDomain_t domainUpperBound;
  ServoDomain_t domainDefaultPosition;
  uint8_t servoDampingFactor;
  ServoTranslationRecord_t translationTable[16];
} ServoChannelConfig_t;

ServoChannel_t *ServoChannel_Create(ServoChannelId_t servoId);
void ServoChannel_Destroy(void);
void ServoChannel_Configure(ServoChannel_t *channel, ServoChannelConfig_t *config);
void ServoChannel_Activate(ServoChannel_t *channel);
flag ServoChannel_IsActive(ServoChannel_t *channel);
void ServoChannel_SetPosition(ServoChannel_t *channel, ServoDomain_t newPosition);
void ServoChannel_Process(ServoChannel_t *channel);

/// Testing related functions.
ServoDomain_t ServoChannel_GetPosition(ServoChannel_t *channel);
ServoRange_t ServoChannel_GetServoRequestPosition(ServoChannel_t *channel);
ServoRange_t ServoChannel_GetServoActualPosition(ServoChannel_t *channel);

/// End of File
