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

/// For Analog Controller 1 (Quadrant)

#define DD_LED_REG  DDRD
#define LED_PORT    PORTD
#define LED_TX      PD4
#define LED_RX      PD5

/// Analog Node Controller 1
#define NODE_ID_STR  "QDC"
#define NODE_VERSION "NI:V1.1"
#define NODE_SERIAL_NUM "SN:0010062"
#define NODE_SPECIFIC_COMMANDS  { "!SetActive:", mpsSetActive },

static uint16_t qdcRateTicks = 0;

static uint16_t nodeWaitTickCount;

#define CHANNEL_COUNT  7
#define FILTER_DEPTH   4

static uint16_t channelSumValue[CHANNEL_COUNT];
static volatile uint16_t channelLastValue[CHANNEL_COUNT];
static uint16_t filterTable[CHANNEL_COUNT][FILTER_DEPTH];
static uint8_t filterStep;

static uint16_t getChannelValue(uint8_t channel)
{
  return channelSumValue[channel] / FILTER_DEPTH;
}

static void runFilter(uint8_t channel, uint16_t value)
{
  // @todo: channels 0 - 5 are external acquisition, channel 6 is the driving voltage.
  //        Acquisition channels should adjust to the driving voltage.

  channelSumValue[channel] = channelSumValue[channel] - filterTable[channel][filterStep] + value;
  filterTable[channel][filterStep] = value;  
}

/// ADC Reference is 2.5V, driving voltage is 5V with a divide by 2 resister network,
/// so nominal full swing is 0 - 2.5V: 0 - 1023.
///
static uint16_t adcAcquire(void)
{
  //    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // ADC enable, start conversion, clock div 128.
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // ADC enable, start conversion, clock div 128.
  ADCSRA |= _BV(ADSC);

  // wait for conversion to complete.
  while ((ADCSRA & _BV(ADSC)) == 1)
    /*null*/ ;

  return ADCW;
}

static void adcSetChannel(uint8_t channel)
{
  ADMUX = (channel & 0x07);   // refs1:refs = 0 for external AREF, ADLAR = 0 for no left justify.
}

static void processAdcChannel(uint8_t channel)
{
  uint16_t value;

  value = adcAcquire();
  channelLastValue[channel] = value;
  runFilter(channel, value);
}

/// Process the next channel.
static void processDataInput(void)
{
  static uint8_t channel = 0;

  processAdcChannel(channel);
  channel += 1;
  if (channel >= CHANNEL_COUNT)
  {
    channel = 0;
    filterStep += 1;
    if (filterStep >= FILTER_DEPTH)
      filterStep = 0;
  }
  adcSetChannel(channel);
}

static void sendQuadrantValues(void)
{
  static char quadrant_message[64];

  sprintf(quadrant_message, "@QUADRANT:%u,%u,%u,%u,%u,%u",
	  getChannelValue(1),
	  getChannelValue(2),
	  getChannelValue(3),
	  getChannelValue(4),
	  getChannelValue(5),
	  getChannelValue(6));

  SerialMessage_Transmit(quadrant_message);
}

#define VALUE_MARGIN 16
static flag fCheckValueChange(void)
{
  return TRUE;   // for debugging
}


static void serviceProcess(void)
{
  processDataInput();   // acquire and process all analog input channels.

  if (qdcRateTicks != 0)
  {
    if (nodeWaitTickCount == 0)
    {
      if (fCheckValueChange())
      {
	// check if any values changed...
	// and transmit
	sendQuadrantValues();

	nodeWaitTickCount = qdcRateTicks;
      }
    }
    else
      nodeWaitTickCount -= 1;
  }
}

static void nodeServiceInitialize(void)
{
  int channel, depth;

  filterStep = 0;
  for (channel = 0; channel < CHANNEL_COUNT; channel++)
  {
    channelSumValue[channel] = 0;
    channelLastValue[channel] = 0;
    for (depth = 0; depth < FILTER_DEPTH; depth++)
      filterTable[channel][depth] = 0;
  }

  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // ADC enable, clock div 128.
  DIDR0 = 0x3F;
  adcSetChannel(channel);
}

static void mpsSetActiveAck(int16_t qdcActiveMs, int16_t qdcRateMs)
{
  char str[32];

  sprintf(str, "@ACK:SetActive:%d,%d", qdcActiveMs, qdcRateMs);
  SerialMessage_Transmit(str);
}

/// !SetActive
static void mpsSetActive(const char *message)
{
  int16_t qdcRateMs;
  int16_t qdcActiveMs;

  // Move past the "!SetActive:"
  message += 11;  // strlen("!SetActive:")
  message = parseIntegerValue(message, &qdcActiveMs);
  message = parseIntegerValue(message, &qdcRateMs);

  if ((qdcActiveMs >= 0) && (qdcRateMs >= 0))
  {
    nodeWaitTickCount = TICKS(qdcActiveMs);
    qdcRateTicks = TICKS(qdcRateMs);

    mpsSetActiveAck(qdcActiveMs, qdcRateMs);
  }
  else
  {
    SerialMessage_Transmit("@NAK:QDC: ActiveDelayMS and RateMS must be positive.");
  }
}

/// Include the main node controller.
#include "NodeCtl.c"

/// End of Analog Node Controller
