/// Cockpit Node Controller firmware
///
/// Servo Channel implementation.

#include "CommonDefs.h"
#include "ServoChannel.h"
#include "ServoPwm.h"

#define START_BIST 99

typedef void (*ProcessRoutine_t)(ServoChannel_t *self);

struct _servo_channel_
{
  ServoChannelConfig_t *servoConfig;
  ServoDomain_t currentDomainPosition;
  ServoRange_t servoRequestPosition;
  ServoRange_t servoActualPosition;
  unsigned int delayCount;
  unsigned int bistIndex;
  ProcessRoutine_t ServoProcessRoutine;
};

static ServoChannel_t servoChannel[NUM_SERVO_CHANNELS];

static void ServoChannel_SetDelayMS(ServoChannel_t *self, int16_t milliseconds)
{
  self->delayCount = (unsigned int)(TICKS(milliseconds) - 1);
}

static flag ServoChannel_IsDelayExpired(ServoChannel_t *self)
{
  return (self->delayCount == 0);
}

static void ServoChannel_DelayTick(ServoChannel_t *self)
{
  self->delayCount -= 1;
}

static void ServoChannel_SetProcess(ServoChannel_t *self, ProcessRoutine_t routine)
{
  self->ServoProcessRoutine = routine;
}

ServoChannel_t *ServoChannel_Create(ServoChannelId_t servoId)
{
  ServoChannel_t *channel = &servoChannel[servoId];

  channel->servoConfig = NULL;
  ServoChannel_SetProcess(channel, NULL);

  return channel;
}

void ServoChannel_Destroy(void)
{
}

flag ServoChannel_IsActive(ServoChannel_t *self)
{
  return (self->ServoProcessRoutine != NULL);
}

void ServoChannel_Configure(ServoChannel_t *self, ServoChannelConfig_t *config)
{
  self->servoConfig = config;
}

void ServoChannel_Process(ServoChannel_t *self)
{
  if (self->ServoProcessRoutine != NULL)
    self->ServoProcessRoutine(self);
}


static ServoRange_t abs_diff(ServoRange_t r1, ServoRange_t r2)
{
  int rangeDiff;

  rangeDiff = (int)r1 - (int)r2;
  if (rangeDiff < 0)
    rangeDiff = -(rangeDiff);

  return (ServoRange_t)rangeDiff;
}

static ServoRange_t calculateDampedPosition(ServoRange_t actualPosition, ServoRange_t requestPosition, uint8_t dampingFactor)
{
  return (ServoRange_t)((((uint32_t)actualPosition * (uint32_t)dampingFactor) +
			 ((uint32_t)requestPosition * (uint32_t)(256U - dampingFactor))) / (uint32_t)256U);

//  return requestPosition;
}

static void ServoChannel_UpdateServoPosition(ServoChannel_t *self)
{
  if (self->servoActualPosition != self->servoRequestPosition)
  {
    if (abs_diff(self->servoActualPosition, self->servoRequestPosition) < 5)
      self->servoActualPosition = self->servoRequestPosition;
    else
      self->servoActualPosition = calculateDampedPosition(self->servoActualPosition,
							  self->servoRequestPosition,
							  self->servoConfig->servoDampingFactor);
    ServoPwm_SetServoPosition(self->servoConfig->servoPwmId, self->servoActualPosition);
  }
}

static void ServoChannel_ProcessActive(ServoChannel_t *self)
{
  if (ServoChannel_IsDelayExpired(self))
  {
    ServoChannel_UpdateServoPosition(self);
    ServoChannel_SetDelayMS(self, 100);
  }
  else
    ServoChannel_DelayTick(self);
}

static void ServoChannel_StartActive(ServoChannel_t *self)
{
  ServoChannel_SetProcess(self, ServoChannel_ProcessActive);
}

static void ServoChannel_SetBistPosition(ServoChannel_t *self)
{
  self->servoActualPosition = self->servoConfig->translationTable[self->bistIndex].servo;
  ServoPwm_SetServoPosition(self->servoConfig->servoPwmId, self->servoActualPosition);
  ServoChannel_SetDelayMS(self, 1000);
}

static void ServoChannel_ProcessBist(ServoChannel_t *self)
{
  if (ServoChannel_IsDelayExpired(self))
  {
    if (self->bistIndex == START_BIST)
    {
      self->bistIndex = 0;
      ServoChannel_SetBistPosition(self);
    }
    else if (self->servoConfig->translationTable[self->bistIndex].domain == self->servoConfig->domainUpperBound)
    {
      ServoChannel_StartActive(self);
    }
    else
    {
      self->bistIndex += 1;
      ServoChannel_SetBistPosition(self);
    }
  }
  else
    ServoChannel_DelayTick(self);
}

static void ServoChannel_StartBist(ServoChannel_t *self)
{
  ServoChannel_SetDelayMS(self, 2500);
  self->bistIndex = START_BIST;
  ServoChannel_SetProcess(self, ServoChannel_ProcessBist);
}

void ServoChannel_Activate(ServoChannel_t *self)
{
  if (self->servoConfig != NULL)
  {
    ServoChannel_SetPosition(self, self->servoConfig->domainDefaultPosition);
    ServoChannel_StartBist(self);
  }
}

#define PRECISION_FACTOR 1024

/// Calculates the servo position from the domain position using the calibration table.
static ServoRange_t interpolateRange(ServoDomain_t domainPosition,
				     ServoDomain_t domainLow, ServoDomain_t domainHigh,
				     ServoRange_t rangeLow, ServoRange_t rangeHigh)
{
  int32_t domainPoint;
  uint32_t rangePoint;

  domainPoint = ((int32_t)PRECISION_FACTOR * (int32_t)(domainPosition - domainLow));
  domainPoint /= (int32_t)(domainHigh - domainLow);

  rangePoint = ((uint32_t)rangeHigh - (uint32_t)rangeLow);
  rangePoint = ((uint32_t)domainPoint * rangePoint) / (uint32_t)PRECISION_FACTOR;
  rangePoint += rangeLow;

  return (ServoRange_t)rangePoint;
}

static ServoRange_t calculateServoPosition(ServoTranslationRecord_t *pTransRecord, ServoDomain_t domainPosition)
{
  ServoRange_t rangePoint;
  int i;

  rangePoint = pTransRecord[0].servo;   // failsafe value
  for (i = 0; i < 16; i++)
  {
    if (domainPosition == pTransRecord[0].domain)
    {
      rangePoint = pTransRecord[0].servo;
      break;
    }
    else if (domainPosition < pTransRecord[1].domain)
    {
      rangePoint = interpolateRange(domainPosition,
				    pTransRecord[0].domain,
				    pTransRecord[1].domain,
				    pTransRecord[0].servo,
				    pTransRecord[1].servo);
      break;
    }
    else
      pTransRecord += 1;
  }
  // Error if i == 16...
  return rangePoint;
}

void ServoChannel_SetPosition(ServoChannel_t *self, ServoDomain_t newDomainPosition)
{
  // Check and control bounds.
  if (newDomainPosition < self->servoConfig->domainLowerBound)
    newDomainPosition = self->servoConfig->domainLowerBound;
  else if (newDomainPosition > self->servoConfig->domainUpperBound)
    newDomainPosition = self->servoConfig->domainUpperBound;

  //  if (self->currentDomainPosition != newDomainPosition)
  {
    self->servoRequestPosition = calculateServoPosition(self->servoConfig->translationTable, newDomainPosition);
    self->currentDomainPosition = newDomainPosition;
  }
}

ServoDomain_t ServoChannel_GetPosition(ServoChannel_t *self)
{
  return self->currentDomainPosition;
}

ServoRange_t ServoChannel_GetServoRequestPosition(ServoChannel_t *self)
{
  return self->servoRequestPosition;
}

ServoRange_t ServoChannel_GetServoActualPosition(ServoChannel_t *self)
{
  return self->servoActualPosition;
}

/// End of File
