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

/// Start FPI (Fuel Pressure Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "FPI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010035"
#define NODE_SPECIFIC_COMMANDS  { "!SetFPI:", mpsSetFPI },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
     0,                   // Lower bound
    25,                   // Upper bound
     0,                   // default value (current barometeric pressure)
    185,                  // damping factor (medium)
    {   {  0,  1750 },    // servo domain, servo range (scaled x100)
	{  5,  2950 },
	{ 10,  4325 },
	{ 15,  5725 },
	{ 20,  7150 },
	{ 25,  8500 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
     0,                   // Lower bound
    25,                   // Upper bound
     0,                   // default value (current barometeric pressure)
    185,                  // damping factor (medium)
    {   {  0,  1750 },    // servo domain, servo range (scaled x100)
	{  5,  2950 },
	{ 10,  4325 },
	{ 15,  5725 },
	{ 20,  7150 },
	{ 25,  8500 }
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

static void mpsSetFpiAck(int16_t fpi0, int16_t fpi1)
{
  char str[32];

  sprintf(str, "@ACK:FPI,%d,%d", fpi0, fpi1);
  SerialMessage_Transmit(str);
}

/// ("SetMPI:%1.0f,%1.0f", engine0_fpi, engine1_fpi);
static void mpsSetFPI(const char *message)
{
  int16_t fpi0, fpi1;
  
  // Move past the "!SetFPI:"
  message += 8;
  message = parseIntegerValue(message, &fpi0);
  message = parseIntegerValue(message, &fpi1);
  ServoChannel_SetPosition(channel_0, fpi0);
  ServoChannel_SetPosition(channel_1, fpi1);

  mpsSetFpiAck(fpi0, fpi1);
}

/// Include the main node controller.
#include "NodeCtl.c"


/// End FPI
