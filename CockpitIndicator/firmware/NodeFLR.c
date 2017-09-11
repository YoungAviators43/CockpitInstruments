/// Cockpit Node Controller firmware
///
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start FLR (Fuel Level Rear Tanks Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "FLR"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010063"
#define NODE_SPECIFIC_COMMANDS  { "!SetFLV:", mpsSetFLV },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
      0,                  // Lower bound
    151,                  // Upper bound
      0,                  // default value
    240,                  // damping factor (very high)
    {   {  0,  6950 },    // servo domain, servo range (scaled x100)
	{ 40,  6000 },
	{ 80,  5000 },
	{120,  3975 },
	{151,  3375 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
      0,                  // Lower bound
    151,                  // Upper bound
      0,                  // default value
    240,                  // damping factor (very high)
    {   {  0,  3100 },    // servo domain, servo range (scaled x100)
	{ 40,  3950 },
	{ 80,  5000 },
	{120,  6060 },
	{151,  6800 }
    }
  };

static ServoChannel_t *channel_0;
static ServoChannel_t *channel_1;
static void nodeServiceInitialize(void)
{
  ServoPwm_Create();
  channel_0 = ServoChannel_Create(SERVO_CHANNEL_0);
  ServoChannel_Configure(channel_0, &config_channel_0);
  ServoChannel_Activate(channel_0);

  channel_1 = ServoChannel_Create(SERVO_CHANNEL_1);
  ServoChannel_Configure(channel_1, &config_channel_1);
  ServoChannel_Activate(channel_1);
}

static void serviceProcess(void)
{
  ServoChannel_Process(channel_0);
  ServoChannel_Process(channel_1);
}

static void mpsSetFlvAck(int16_t level0, int16_t level1)
{
  char str[32];

  sprintf(str, "@ACK:FLR,%d,%d", level0, level1);
  SerialMessage_Transmit(str);
}

/// ("SetFLV:%1.0f,%1.0f", tank0_level, tank1_level);
static void mpsSetFLV(const char *message)
{
  int16_t level0, level1;
  
  // Move past the "!SetFLV:"
  message += 8;
  message = parseIntegerValue(message, &level0);
  message = parseIntegerValue(message, &level1);
  ServoChannel_SetPosition(channel_0, level0);
  ServoChannel_SetPosition(channel_1, level1);

  mpsSetFlvAck(level0, level1);
}

/// Include the main node controller.
#include "NodeCtl.c"


/// End FLR
