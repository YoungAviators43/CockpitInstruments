

#include "CommonDefs.h"
#include "StepControl.h"

#define STEPS_PER_REV    48U
#define USTEPS_PER_STEP   8U
#define STEPS_FAST_PER_THOUSAND  (STEPS_PER_REV)
#define STEPS_MICRO_PER_THOUSAND (STEPS_PER_REV * USTEPS_PER_STEP)

#define CONTROL_PORT  PORTC

static void StepperInit(void)
{
}

inline static void Stepper_ClockPulse(void)
{
}

static void Stepper_Forward(void)
{
  // set direction to forward
  Stepper_ClockPulse();
}

static void Stepper_Reverse(void)
{
  // set direction to reverse
  Stepper_ClockPulse();
}

static void Stepper_SetStepModeSkew(void)
{
}

static void Stepper_SetStepModeNormal(void)
{
}


typedef void (*ProcessRoutine_t)(void);

static uint32_t Step_ConvertFeetToFastSteps(uint16_t feet)
{
  return (feet * STEPS_FAST_PER_THOUSAND) / 1000U;
}

static uint32_t Step_ConvertFeetToMicroSteps(uint16_t feet)
{
  return (feet * STEPS_MICRO_PER_THOUSAND) / 1000U;
}

static ProcessRoutine_t stepProcessRoutine = NULL;
static volatile uint32_t currentStepPosition;
static volatile uint32_t desiredStepPosition;

static void Stepper_SetProcess(ProcessRoutine_t routine)
{
  stepProcessRoutine = routine;
}

/*API*/void Stepper_Process(void)
{
  if (stepProcessRoutine != NULL)
    stepProcessRoutine();
}

/*API*/void Stepper_SetPosition(uint16_t position)
{
  desiredStepPosition = Step_ConvertFeetToMicroSteps(position);
}

static uint8_t IsHomePosition(void)
{
  // read interrupter flag port.
  return FALSE;
}

void Stepper_ProcessActive(void)
{
  if (desiredStepPosition > currentStepPosition)
  {
    currentStepPosition += 1;
    Stepper_Forward();
  }
  else if (desiredStepPosition < currentStepPosition)
  {
    currentStepPosition -= 1;
    Stepper_Reverse();
  }
}

static uint32_t feetAtHome = 1600U;    // calibration value ***

typedef enum {
  HOME_PHASE_COMPLETE,
  HOME_INIT,
  HOME_SKEW_FORWARD,
  HOME_SKEW_REVERSE,
  HOME_FIND_MARKER,
  HOME_FIND_ZERO,
  HOME_SETTLE_ZERO
} StepHomePhase_t;

static StepHomePhase_t stepHomePhase;

uint8_t IsHomeProcessComplete(void)
{
  return (stepHomePhase == HOME_PHASE_COMPLETE);
}

static unsigned int stepDelayCount;

static flag Stepper_IsDelayExpired(void)
{
  return (stepDelayCount == 0);
}

static void Stepper_SetDelayMS(int16_t milliseconds)
{
  stepDelayCount = (unsigned int)(TICKS(milliseconds) - 1);
}

static void Stepper_DelayTick(void)
{
  stepDelayCount -= 1;
}

static void Step_ProcessHoming(void)
{
  switch (stepHomePhase)
  {
    case HOME_INIT:
      if (Stepper_IsDelayExpired())
      {
        Stepper_SetStepModeSkew();
        if (IsHomePosition())
        {
          currentStepPosition = 0;
          stepHomePhase = HOME_SKEW_FORWARD;
        }
        else
        {
          stepHomePhase = HOME_SKEW_REVERSE;
        }
      }
      else
      {
        Stepper_DelayTick();
      }
      break;

    case HOME_SKEW_FORWARD:
      // Step forward until requested steps exhausted.
      if (currentStepPosition < Step_ConvertFeetToFastSteps(feetAtHome + 1000))
      {
        Stepper_Forward();
        currentStepPosition += 1;
      }
      else
      {
        stepHomePhase = HOME_SKEW_REVERSE;
      }
      break;

    case HOME_SKEW_REVERSE:
      // Skew reverse until homing marker reached.
      if (!IsHomePosition())
      {
        Stepper_Reverse();
      }
      else
      {
        Stepper_SetStepModeNormal();
        Stepper_SetDelayMS(100);      // use 100ms settle time
        stepHomePhase = HOME_FIND_MARKER;
      }
      break;

    case HOME_FIND_MARKER:
      // Slowly micro-step forward until the homing marker clears.
      if (Stepper_IsDelayExpired())
      {
        if (IsHomePosition())
        {
          Stepper_Forward();
          Stepper_SetDelayMS(100);
        }
        else
        {
          // Marker found.  Set position to calibrated mark.
          currentStepPosition = Step_ConvertFeetToMicroSteps(feetAtHome);
          stepHomePhase = HOME_FIND_ZERO;
        }
      }
      else
      {
        Stepper_DelayTick();
      }
      break;

    case HOME_FIND_ZERO:
      // Move to zero position.
      if (currentStepPosition > 0)
      {
        Stepper_Reverse();
        currentStepPosition -= 1;
      }
      else
      {
        Stepper_SetDelayMS(2500);
        stepHomePhase = HOME_SETTLE_ZERO;
      }
      break;

    case HOME_SETTLE_ZERO:
      // Sit at zero for a long settling interval.
      if (Stepper_IsDelayExpired())
      {
        // Start normal position processing.
        Stepper_SetProcess(Stepper_ProcessActive);
        stepHomePhase = HOME_PHASE_COMPLETE;
      }
      else
      {
        Stepper_DelayTick();
      }
      break;

    case HOME_PHASE_COMPLETE:
      // Do nothing.  Should never get here.
      break;
  }
}

/*API*/void startHomingSequence(void)
{
  StepperInit();
  Stepper_SetDelayMS(1000);
  stepHomePhase = HOME_INIT;
  Stepper_SetProcess(Step_ProcessHoming);
}


