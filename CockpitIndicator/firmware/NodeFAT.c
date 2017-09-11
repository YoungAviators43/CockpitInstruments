/// Cockpit Node Controller firmware for FAT (Free Air Temperature Indicator)
///
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start FAT (Free Air Temperature Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "FAT"
#define NODE_VERSION "NI:V1.2"
#define NODE_SERIAL_NUM "SN:0010073"
#define NODE_SPECIFIC_COMMANDS  { "!SetFAT:", mpsSetFAT },

/// Servo 50% is 50 degrees C
static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
    -50,                  // Lower bound
    150,                  // Upper bound
     25,                  // default value (typical ambient)
    220,                  // damping factor (medium high)
    {   { -50,  3750 },   // servo domain, servo range (scaled x100)
	{   0,  4300 },
	{  50,  5000 },
	{ 100,  5750 },
	{ 150,  6400 }
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

static void mpsSetFatAck(int16_t temperatureC)
{
  char str[32];

  sprintf(str, "@ACK:FAT,%d", temperatureC);
  SerialMessage_Transmit(str);
}

/// ("!SetFAT:%1.0f", temperatureC)
static void mpsSetFAT(const char *message)
{
  int16_t temperatureC;
  
  // Move past the "!SetFAT:"
  message += 8;
  message = parseIntegerValue(message, &temperatureC);
  ServoChannel_SetPosition(channel_0, temperatureC);

  mpsSetFatAck(temperatureC);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End FAT
