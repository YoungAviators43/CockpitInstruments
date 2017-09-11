/// Cockpit Node Controller firmware for ALT (Altimeter)
///
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start ALT (Altimeter)

#include "StepControl.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "ALT"
#define NODE_VERSION "NI:V1.2"
#define NODE_SERIAL_NUM "SN:0010107"
#define NODE_SPECIFIC_COMMANDS  { "!SetALT:", mpsSetALT },

static void nodeServiceInitialize(void)
{
  startHomingSequence();
}

static void serviceProcess(void)
{
  Stepper_Process();
}

static void mpsSetAltAck(int16_t altitudeFee)
{
  char str[32];

  sprintf(str, "@ACK:" NODE_ID_STR ",%d", altitudeFeet);
  SerialMessage_Transmit(str);
}

/// ("!SetALT:%1.0f", altitudeFeet)
static void mpsSetALT(const char *message)
{
  int16_t altitudeFeet;
  
  // Move past the "!SetALT:"
  message += 8;
  message = parseIntegerValue(message, &altitudeFeet);
  Stepper_SetPosition(altitudeFeet);

  mpsSetAltAck(altitudeFeet);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End ALT
