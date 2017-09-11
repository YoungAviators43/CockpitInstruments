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

#define NODE_ID_STR  "SUI"
#define NODE_VERSION "NI:V1.2"
#define NODE_SERIAL_NUM "SN:0010087"
#define NODE_SPECIFIC_COMMANDS  { "!SetSUC:", mpsSetSUC },

/// Servo 50% is 5 HgI
/// Value is HgI * 100.
static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
       0,                 // Lower bound
    1000,                 // Upper bound
       0,                 // default value (engines off)
     180,                 // damping factor (medium)
    {   {      0,   500 },  // servo domain, servo range (scaled x100)
	{    200,  2000 },
	{    500,  5000 },
	{    800,  8000 },
	{   1000,  9900 }
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

static void mpsSetHgiAck(int16_t suctionHgi)
{
  char str[32];

  sprintf(str, "@ACK:" NODE_ID_STR ",%d", suctionHgi);
  SerialMessage_Transmit(str);
}

/// ("!SetSUC:%1.0f", suctionHgi * 100)
static void mpsSetSUC(const char *message)
{
  int16_t suctionHgi;
  
  // Move past the "!SetSUC:"
  message += 8;
  message = parseIntegerValue(message, &suctionHgi);
  ServoChannel_SetPosition(channel_0, suctionHgi);

  mpsSetHgiAck(suctionHgi);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End BPI
