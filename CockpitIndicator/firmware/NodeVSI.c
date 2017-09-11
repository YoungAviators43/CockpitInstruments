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

/// Start VSI (Vertical Speed Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "VSI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010041"
#define NODE_SPECIFIC_COMMANDS  { "!SetVSI:", mpsSetVSI },

/// Servo 50% is 0 ft/sec climb
static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
    -600,                  // Lower bound
     600,                  // Upper bound
       0,                  // default value (typical ambient)
     140,                  // damping factor (medium low)
    {   { -600,  700 },    // servo domain, servo range (scaled x100)
	{ -400, 1500 },
	{ -200, 2600 },
	{ -100, 3500 },
	{  -50, 4100 },
	{    0, 5000 },
	{   50, 5900 },
	{  100, 6700 },
	{  200, 7700 },
	{  400, 8800 },
	{  600, 9700 }
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

static void mpsSetVsiAck(int16_t verticalSpeed)
{
  char str[32];

  sprintf(str, "@ACK:VSI,%d", verticalSpeed);
  SerialMessage_Transmit(str);
}

/// ("!SetVSI:%1.0f", verticalSpeed)
static void mpsSetVSI(const char *message)
{
  int16_t verticalSpeed;
  
  // Move past the "!SetVSI:"
  message += 8;
  message = parseIntegerValue(message, &verticalSpeed);
  ServoChannel_SetPosition(channel_0, verticalSpeed);

  mpsSetVsiAck(verticalSpeed);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End ASI
