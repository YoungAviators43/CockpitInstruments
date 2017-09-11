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

/// Start OPI (Oil Pressure Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "OPI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010048"
#define NODE_SPECIFIC_COMMANDS  { "!SetOPR:", mpsSetOPR },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
      0,                  // Lower bound
    200,                  // Upper bound
      0,                  // default value
    160,                  // damping factor (medium low)
    {   {  0,  1775 },    // servo domain, servo range (scaled x100)
	{ 50,  3375 },
	{100,  5000 },
	{150,  6725 },
	{200,  8450 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
      0,                  // Lower bound
    200,                  // Upper bound
      0,                  // default value
    160,                  // damping factor (medium low)
    {   {  0,  1575 },    // servo domain, servo range (scaled x100)
	{ 50,  3175 },
	{100,  5000 },
	{150,  6900 },
	{200,  8700 }
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

static void mpsSetOprAck(int16_t opr0, int16_t opr1)
{
  char str[32];

  sprintf(str, "@ACK:OPR,%d,%d", opr0, opr1);
  SerialMessage_Transmit(str);
}

/// ("SetOPR:%1.0f,%1.0f", engine0_opr, engine1_opr);
static void mpsSetOPR(const char *message)
{
  int16_t opr0, opr1;
  
  // Move past the "!SetOTR:"
  message += 8;
  message = parseIntegerValue(message, &opr0);
  message = parseIntegerValue(message, &opr1);
  ServoChannel_SetPosition(channel_0, opr0);
  ServoChannel_SetPosition(channel_1, opr1);

  mpsSetOprAck(opr0, opr1);
}

/// Include the main node controller.
#include "NodeCtl.c"


/// End OTI
