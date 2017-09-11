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

/// Start RPM (Tachometer)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "RPM"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010031"
#define NODE_SPECIFIC_COMMANDS  { "!SetRPM:", mpsSetRPM },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
    0,                    // Lower bound
    4500,                 // Upper bound
    0,                    // default value
    150,                  // damping factor (medium low)
    {   {    0,  1325 },  // servo domain, servo range (scaled x100)
	{  500,  2000 },
	{ 1000,  2775 },
	{ 1500,  3625 },
	{ 2000,  4600 },
	{ 2500,  5450 },
	{ 3000,  6350 },
	{ 3500,  7350 },
	{ 4000,  8350 },
	{ 4500,  9200 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
    0,                    // Lower bound
    4500,                 // Upper bound
    0,                    // default value
    150,                  // damping factor (medium low)
    {   {    0,  1325 },  // servo domain, servo range (scaled x100)
	{  500,  2000 },
	{ 1000,  2850 },
	{ 1500,  3680 },
	{ 2000,  4625 },
	{ 2500,  5480 },
	{ 3000,  6425 },
	{ 3500,  7450 },
	{ 4000,  8425 },
	{ 4500,  9525 }
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

static void mpsSetRpmAck(int16_t rpm0, int16_t rpm1)
{
  char str[32];

  sprintf(str, "@ACK:RPM,%d,%d", rpm0, rpm1);
  SerialMessage_Transmit(str);
}

/// ("SetRPM:%1.0f,%1.0f", engine0_rpm, engine1_rpm);
static void mpsSetRPM(const char *message)
{
  int16_t rpm0, rpm1;
  
  // Move past the "!SetRPM:"
  message += 8;
  message = parseIntegerValue(message, &rpm0);
  message = parseIntegerValue(message, &rpm1);
  ServoChannel_SetPosition(channel_0, rpm0);
  ServoChannel_SetPosition(channel_1, rpm1);

  mpsSetRpmAck(rpm0, rpm1);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End RPM
