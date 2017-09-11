/// Cockpit Node Controller firmware
///

#include "CommonDefs.h"
#include "SerialMessage.h"
#include "TimerService.h"
#include "MessageParse.h"

#include <string.h>
#include <stdio.h>

#define FIRMWARE_ID_STR  "FW:V1.2"

/// Start MPI (Manifold Pressure Indicator)

#include "ServoPwm.h"
#include "ServoChannel.h"

#define DD_LED_REG  DDRC
#define LED_PORT    PORTC
#define LED_TX      PC0
#define LED_RX      PC1

#define NODE_ID_STR  "MPI"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010021"
#define NODE_SPECIFIC_COMMANDS  { "!SetMPI:", mpsSetMPI },

static ServoChannelConfig_t config_channel_0 =
  {
    ServoPwm_A,
    10,                   // Lower bound
    75,                   // Upper bound
    30,                   // default value (current barometeric pressure)
    175,                  // damping factor (medium low)
    {   { 10,   900 },    // servo domain, servo range (scaled x100)
	{ 20,  2200 },
	{ 30,  3425 },
	{ 40,  4750 },
	{ 50,  6050 },
	{ 60,  7350 },
	{ 70,  8800 },
	{ 75,  9400 }
    }
  };

static ServoChannelConfig_t config_channel_1 =
  {
    ServoPwm_B,
    10,                   // Lower bound
    75,                   // Upper bound
    30,                   // default value (current barometeric pressure)
    175,                  // damping factor (medium low)
    {   { 10,   900 },    // servo domain, servo range (scaled x100)
	{ 20,  2200 },
	{ 30,  3425 },
	{ 40,  4750 },
	{ 50,  6050 },
	{ 60,  7350 },
	{ 70,  8800 },
	{ 75,  9400 }
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

static void mpsSetMpiAck(int16_t mpi0, int16_t mpi1)
{
  char str[32];

  sprintf(str, "@ACK:MPI,%d,%d", mpi0, mpi1);
  SerialMessage_Transmit(str);
}

/// ("SetMPI:%4.0f,%4.0f", engine0_mpi, engine1_mpi);
static void mpsSetMPI(const char *message)
{
  int16_t mpi0, mpi1;
  
  // Move past the "!SetMPI:"
  message += 8;
  message = parseIntegerValue(message, &mpi0);
  message = parseIntegerValue(message, &mpi1);
  ServoChannel_SetPosition(channel_0, mpi0);
  ServoChannel_SetPosition(channel_1, mpi1);

  mpsSetMpiAck(mpi0, mpi1);
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End MPI
