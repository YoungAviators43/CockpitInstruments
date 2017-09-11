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

/// Start OTI (Oil Temperature Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "OTI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010038"
#define NODE_SPECIFIC_COMMANDS  { "!SetOTP:", mpsSetOTP },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
    -50,                  // Lower bound
    150,                  // Upper bound
      0,                  // default value
    225,                  // damping factor (medium high)
    {   {-50,  7250 },    // servo domain, servo range (scaled x100)
	{  0,  6150 },
	{ 50,  5000 },
	{100,  3850 },
	{150,  2750 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
    -50,                  // Lower bound
    150,                  // Upper bound
      0,                  // default value
    225,                  // damping factor (medium high)
    {   {-50,  3000 },    // servo domain, servo range (scaled x100)
	{  0,  4000 },
	{ 50,  5000 },
	{100,  6000 },
	{150,  7250 }
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

static void mpsSetOtpAck(int16_t fpi0, int16_t fpi1)
{
  char str[32];

  sprintf(str, "@ACK:OTI,%d,%d", fpi0, fpi1);
  SerialMessage_Transmit(str);
}

/// ("SetOTP:%1.0f,%1.0f", engine0_otp, engine1_otp);
static void mpsSetOTP(const char *message)
{
  int16_t otp0, otp1;
  
  // Move past the "!SetOTP:"
  message += 8;
  message = parseIntegerValue(message, &otp0);
  message = parseIntegerValue(message, &otp1);
  ServoChannel_SetPosition(channel_0, otp0);
  ServoChannel_SetPosition(channel_1, otp1);

  mpsSetOtpAck(otp0, otp1);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End OTI
