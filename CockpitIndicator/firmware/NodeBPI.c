/// Cockpit Node Controller firmware for BPI (Brake Pressure Indicator)
///
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start BPI (Brake Pressure Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "BPI"
#define NODE_VERSION "NI:V1.2"
#define NODE_SERIAL_NUM "SN:0010087"
#define NODE_SPECIFIC_COMMANDS  { "!SetBPI:", mpsSetBPI },

/// Servo 50% is 1000PSI
static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
       0,                 // Lower bound
    2000,                 // Upper bound
       0,                 // default value (engines off)
     180,                 // damping factor (medium)
    {   {    0,  2000 },  // servo domain, servo range (scaled x100)
	{  800,  4400 },
	{ 1000,  5000 },
	{ 1200,  5600 },
	{ 2000,  7800 }
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

static void mpsSetBpiAck(int16_t brakePsi)
{
  char str[32];

  sprintf(str, "@ACK:" NODE_ID_STR ",%d", brakePsi);
  SerialMessage_Transmit(str);
}

/// ("!SetBPI:%1.0f", brakePsi)
static void mpsSetBPI(const char *message)
{
  int16_t brakePsi;
  
  // Move past the "!SetBPI:"
  message += 8;
  message = parseIntegerValue(message, &brakePsi);
  ServoChannel_SetPosition(channel_0, brakePsi);

  mpsSetBpiAck(brakePsi);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End BPI
