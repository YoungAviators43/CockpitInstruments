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

/// Start CYI (Cylinder Head Temperature)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "CYI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010042"
#define NODE_SPECIFIC_COMMANDS  { "!SetCYL:", mpsSetCYL },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
      0,                  // Lower bound
    400,                  // Upper bound
     35,                  // default value (typical ambient)
    225,                  // damping factor (medium high)
    {   {  0,  6900 },    // servo domain, servo range (scaled x100)
	{ 50,  6350 },
	{100,  5900 },
	{150,  5425 },
	{200,  5000 },
	{250,  4525 },
	{300,  4050 },
	{350,  3625 },
	{400,  3210 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
      0,                  // Lower bound
    400,                  // Upper bound
     35,                  // default value (typical ambient)
    225,                  // damping factor (medium high)
    {   {  0,  3275 },    // servo domain, servo range (scaled x100)
	{ 50,  3675 },
	{100,  4150 },
	{150,  4575 },
	{200,  5000 },
	{250,  5425 },
	{300,  5925 },
	{350,  6375 },
	{400,  6900 }
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

static void mpsSetCylAck(int16_t cyl0, int16_t cyl1)
{
  char str[32];

  sprintf(str, "@ACK:CYL,%d,%d", cyl0, cyl1);
  SerialMessage_Transmit(str);
}

/// ("SetCYL:%1.0f,%1.0f", engine0_cyl, engine1_cyl);
static void mpsSetCYL(const char *message)
{
  int16_t cyl0, cyl1;
  
  // Move past the "!SetCYL:"
  message += 8;
  message = parseIntegerValue(message, &cyl0);
  message = parseIntegerValue(message, &cyl1);
  ServoChannel_SetPosition(channel_0, cyl0);
  ServoChannel_SetPosition(channel_1, cyl1);

  mpsSetCylAck(cyl0, cyl1);
}

/// Include the main node controller.
#include "NodeCtl.c"


/// End CYL
