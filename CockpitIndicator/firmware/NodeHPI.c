/// Cockpit Node Controller firmware for HPI (Hydraulic Pressure Indicator)
///
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start HPI (Hydraulic Pressure Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "HPI"
#define NODE_VERSION "NI:V1.2"
#define NODE_SERIAL_NUM "SN:0010086"
#define NODE_SPECIFIC_COMMANDS  { "!SetHYD:", mpsSetHYD },

/// Servo 50% is 1000PSI
static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
       0,                 // Lower bound
    2000,                 // Upper bound
       0,                 // default value (engines off)
     180,                 // damping factor (medium)
    {   {    0,  2300 },  // servo domain, servo range (scaled x100)
	{  800,  4500 },
	{ 1000,  5000 },
	{ 1200,  5500 },
	{ 2000,  8000 }
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

static void mpsSetHpiAck(int16_t hydraulicPsi)
{
  char str[32];

  sprintf(str, "@ACK:" NODE_ID_STR ",%d", hydraulicPsi);
  SerialMessage_Transmit(str);
}

/// ("!SetHYD:%1.0f", hydraulicPsi)
static void mpsSetHYD(const char *message)
{
  int16_t hydraulicPsi;
  
  // Move past the "!SetHPI:"
  message += 8;
  message = parseIntegerValue(message, &hydraulicPsi);
  ServoChannel_SetPosition(channel_0, hydraulicPsi);

  mpsSetHpiAck(hydraulicPsi);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End HPI
