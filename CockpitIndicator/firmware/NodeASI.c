/// Cockpit Node Controller firmware for ASI (Air Speed Indicator)
///
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start ASI (Airspeed Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "ASI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010012"
#define NODE_SPECIFIC_COMMANDS  { "!SetIAS:", mpsSetIAS },

/// Servo 50% is 160mph
static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
      0,                  // Lower bound
    300,                  // Upper bound
      0,                  // default value (typical ambient)
    180,                  // damping factor (medium)
    {   {  0,  1150 },    // servo domain, servo range (scaled x100)
	{ 20,  1600 },
	{ 40,  2050 },
	{ 60,  2500 },
	{ 80,  3000 },
	{100,  3480 },
	{120,  3980 },
	{140,  4500 },
	{160,  5000 },
	{180,  5550 },
	{200,  6100 },
	{220,  6600 },
	{240,  7100 },
	{260,  7600 },
	{280,  8100 },
	{300,  8600 }
    }
  };

static ServoChannel_t *channel_0;
static void nodeServiceInitialize(void)
{
  ServoPwm_Create();
  channel_0 = ServoChannel_Create(SERVO_CHANNEL_0);
  ServoChannel_Configure(channel_0, &config_channel_0);
  ServoChannel_Activate(channel_0);
}

static void serviceProcess(void)
{
  ServoChannel_Process(channel_0);
}

static void mpsSetIasAck(int16_t airSpeed)
{
  char str[32];

  sprintf(str, "@ACK:IAS,%d", airSpeed);
  SerialMessage_Transmit(str);
}

/// ("!SetIAS:%1.0f", airSpeed)
static void mpsSetIAS(const char *message)
{
  int16_t airSpeed;
  
  // Move past the "!SetIAS:"
  message += 8;
  message = parseIntegerValue(message, &airSpeed);
  ServoChannel_SetPosition(channel_0, airSpeed);

  mpsSetIasAck(airSpeed);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End ASI
