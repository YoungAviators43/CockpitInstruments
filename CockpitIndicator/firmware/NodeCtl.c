/// Cockpit Node Controller firmware common/main program.
///
/// This file is directly included into each different node type controller.
///

#ifdef AVR_HAL

#include <string.h>
#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#ifndef FIRMWARE_ID_STR
#define FIRMWARE_ID_STR  "FW:Vx.x"
#endif

void Led_TransmitOn(void)
{
  LED_PORT |= _BV(LED_TX);
}

void Led_TransmitOff(void)
{
  LED_PORT &= ~(_BV(LED_TX));
}

void Led_ReceiveOn(void)
{
  LED_PORT |= _BV(LED_RX);
}

void Led_ReceiveOff(void)
{
  LED_PORT &= ~(_BV(LED_RX));
}

static void Led_Heartbeat(void)
{
  static uint8_t heartbeatCount = 0;

  heartbeatCount += 1;
  if (heartbeatCount >= 100)
  {
    PORTD &= ~(_BV(PD3));
    heartbeatCount = 0;
  }
  else if (heartbeatCount >= 95)
  {
    PORTD |= (_BV(PD3));
  }
}

static void Led_Init(void)
{
  DD_LED_REG |= _BV(LED_TX) | _BV(LED_RX);
  Led_TransmitOn();
  Led_ReceiveOn();

  // Heartbeat LED
  DDRD |= _BV(DDD3);
  PORTD |= _BV(PD3);
}

static char fNodeStateReset;

#define NODE_IDENTIFY_RESET   "@IDENTIFY:" NODE_ID_STR ",RESET," FIRMWARE_ID_STR "," NODE_VERSION "," NODE_SERIAL_NUM
#define NODE_IDENTIFY_ACTIVE  "@IDENTIFY:" NODE_ID_STR ",ACTIVE," FIRMWARE_ID_STR "," NODE_VERSION "," NODE_SERIAL_NUM
#define NODE_STATUS_RESET     "@STATUS:"   NODE_ID_STR ",RESET"
#define NODE_STATUS_ACTIVE    "@STATUS:"   NODE_ID_STR ",ACTIVE"


static void mpsIdentifyReq(const char *message)
{
  char *pResponse = NODE_IDENTIFY_ACTIVE;

  if (fNodeStateReset)
  {
    pResponse = NODE_IDENTIFY_RESET;
    fNodeStateReset = FALSE;
  }

  SerialMessage_Transmit(pResponse);
}

static void mpsStatusReq(const char *message)
{
  char *pResponse = NODE_STATUS_ACTIVE;

  if (fNodeStateReset)
  {
    pResponse = NODE_STATUS_RESET;
    fNodeStateReset = FALSE;
  }

  SerialMessage_Transmit(pResponse);
}

static void mpsUnknownReq(const char *message)
{
  SerialMessage_Transmit("@NAK:" NODE_ID_STR ": Unknown command.");
}

static CommandRecord_t TestCommandTable[] =
  { { "!IDENTIFY", mpsIdentifyReq },
    { "!STATUS",   mpsStatusReq   },

    NODE_SPECIFIC_COMMANDS

    { NULL,        mpsUnknownReq  }
  };

void MessageProcess_Decode(void)
{
  SerialMessage_Process(TestCommandTable);
}

static volatile uint32_t TimerTickCount;

static void TimerProcess(void)
{
  while (TimerTickCount > 0)
  {
    Led_Heartbeat();
    serviceProcess();

    cli();  /*interrupt disable*/
    TimerTickCount -= 1;
    sei();  /*interrupt restore*/
  }
}

/// Interrupt context
static void TimerTickService(void)
{
  TimerTickCount += 1;
}

static void nodeTimerInitialize(void)
{
  TimerTickCount = 0;
  TimerService_Create();
  TimerService_Activate(TICK_RATE, TimerTickService);
}

void sleepNow(void)
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
}

int main(void)
{
  fNodeStateReset = TRUE;
  Led_Init();
  SerialMessage_Create();
  SerialMessage_ReceiveEnable();
  nodeServiceInitialize();
  nodeTimerInitialize();

  sei();  // global enable interrupts
  while (1)
  {
    // Sleep until interrupt...
    sleepNow();
    MessageProcess_Decode();
    TimerProcess();
  }

  return 0;
}

#endif
