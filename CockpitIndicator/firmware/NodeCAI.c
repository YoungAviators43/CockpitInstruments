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

/// Start CAI (Carburator Air Temperature Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "CAI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010046"
#define NODE_SPECIFIC_COMMANDS  { "!SetCAT:", mpsSetCAT },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
    -50,                  // Lower bound
    150,                  // Upper bound
      0,                  // default value
    225,                  // damping factor (medium high)
    {   {-50,  7160 },    // servo domain, servo range (scaled x100)
	{  0,  6100 },
	{ 50,  5000 },
	{100,  3940 },
	{150,  2890 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
    -50,                  // Lower bound
    150,                  // Upper bound
      0,                  // default value
    225,                  // damping factor (medium high)
    {   {-50,  2960 },    // servo domain, servo range (scaled x100)
	{  0,  3920 },
	{ 50,  5000 },
	{100,  6060 },
	{150,  7060 }
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

static void mpsSetCatAck(int16_t cat0, int16_t cat1)
{
  char str[32];

  sprintf(str, "@ACK:CAT,%d,%d", cat0, cat1);
  SerialMessage_Transmit(str);
}

/// ("SetCAT:%1.0f,%1.0f", engine0_cat, engine1_cat);
static void mpsSetCAT(const char *message)
{
  int16_t cat0, cat1;
  
  // Move past the "!SetCAT:"
  message += 8;
  message = parseIntegerValue(message, &cat0);
  message = parseIntegerValue(message, &cat1);
  ServoChannel_SetPosition(channel_0, cat0);
  ServoChannel_SetPosition(channel_1, cat1);

  mpsSetCatAck(cat0, cat1);
}

/// Include the main node controller.
#include "NodeCtl.c"


/// End CAI
