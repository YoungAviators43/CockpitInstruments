/// File: CockpitMPI.cxx
///
/// Cockpit Quadrant Controls

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitQuadrant.hxx"

extern unsigned int logLevel;

/// Work arounds for cockpit condition
#define OSHKOSH

class CockpitQDC : public CockpitNode
{
private:
  PropertyNode *pEngine0_ThrottlePropertyNode;
  PropertyNode *pEngine0_MixturePropertyNode;
  PropertyNode *pEngine0_PropPropertyNode;
  PropertyNode *pEngine1_ThrottlePropertyNode;
  PropertyNode *pEngine1_MixturePropertyNode;
  PropertyNode *pEngine1_PropPropertyNode;

protected:
  void processNodeResponse(char *message);
  void fetchNodeMessage(char *buffer);

public:
  CockpitQDC(CockpitNodeComm_t *pCommChannel);

  const char *nodeTypeCode(void) { return "QDC"; };
  const char *nodeFullName(void) { return "Main Quadrant Control"; };
};

CockpitNode *createCockpitQDC(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitQDC(pCommChannel);
}

static const char *engine0_ThrottlePropertyName = "/controls/engines/engine[0]/throttle";
static const char *engine0_MixturePropertyName = "/controls/engines/engine[0]/mixture";
static const char *engine0_PropPropertyName = "/controls/engines/engine[0]/propeller-pitch";
static const char *engine1_ThrottlePropertyName = "/controls/engines/engine[1]/throttle";
static const char *engine1_MixturePropertyName = "/controls/engines/engine[1]/mixture";
static const char *engine1_PropPropertyName = "/controls/engines/engine[1]/propeller-pitch";

CockpitQDC::CockpitQDC(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "MainQuadrant")
{
  if (logLevel > 1)
    cout << "Creating Main Quadrant Control." << endl;

  pEngine0_ThrottlePropertyNode = acquireCockpitNodeProperty(engine0_ThrottlePropertyName, propFloat);
  pEngine0_MixturePropertyNode = acquireCockpitNodeProperty(engine0_MixturePropertyName, propFloat);
  pEngine0_PropPropertyNode = acquireCockpitNodeProperty(engine0_PropPropertyName, propFloat);
  pEngine1_ThrottlePropertyNode = acquireCockpitNodeProperty(engine1_ThrottlePropertyName, propFloat);
  pEngine1_MixturePropertyNode = acquireCockpitNodeProperty(engine1_MixturePropertyName, propFloat);
  pEngine1_PropPropertyNode = acquireCockpitNodeProperty(engine1_PropPropertyName, propFloat);
}

void CockpitQDC::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetActive:2500,100");  // delay 2500ms, then deliver every 100ms.
}


static char isWhiteSpace(char ch)
{
  return (ch == ' ');  // anything else allowed?
}

typedef long unsigned int uint32_t;

static char *getValue(char *pMessage, int32_t *pValue)
{
  while (isWhiteSpace(*pMessage))
    pMessage += 1;
  *pValue = 0;
  while (isDecimalNumber(*pMessage))
  {
    *pValue = *pValue * 10 + (*pMessage - '0');
    pMessage += 1;
  }
  if (*pMessage == ',')
  {
    pMessage += 1;
  }
  return pMessage;
}

typedef struct { int32_t value; int32_t scale; } Scale_t;

/// Current implementation allows only monotonically increasing values in table.
static Scale_t engine0_throttle_scale[5] = { {   0,   0},
					     {  56,  25},
					     { 100,  50},
					     { 132,  75},
					     { 141, 100} };
static Scale_t engine0_mixture_scale[5] =  { {   0,   0},
					     {  80,  25},
					     { 167,  50},
					     { 243,  75},
					     { 284, 100} };
static Scale_t engine0_prop_scale[5] =     { {   0,   0},
					     {  51,  25},
					     { 163,  50},
					     { 226,  75},
					     { 279, 100} };
static Scale_t engine1_throttle_scale[5] = { {   0,   0},
					     {  58,  25},
					     { 139,  50},
					     { 205,  75},
					     { 346, 100} };
static Scale_t engine1_mixture_scale[5] =  { { 250,   0},
					     { 870,  25},
					     { 911,  50},
					     { 920,  75},
					     { 935, 100} };
static Scale_t engine1_prop_scale[5] =     { {  0,   0},
					     {  5,  25},
					     { 23,  50},
					     { 48,  75},
					     { 65, 100} };
  
static int32_t interpStep(int32_t value, int32_t vLower, int32_t vUpper, int32_t sLower, int32_t sUpper)
{
  return sLower + ((sUpper - sLower) * (value - vLower)) / (vUpper - vLower);
}

static int32_t interp(int32_t value, Scale_t *pTable)
{
  uint32_t interpValue;
  
  if (value < pTable[0].value)
    interpValue = pTable[0].scale;
  else if (value < pTable[1].value)
    interpValue = interpStep(value, pTable[0].value, pTable[1].value, pTable[0].scale, pTable[1].scale);
  else if (value < pTable[2].value)
    interpValue = interpStep(value, pTable[1].value, pTable[2].value, pTable[1].scale, pTable[2].scale);
  else if (value < pTable[3].value)
    interpValue = interpStep(value, pTable[2].value, pTable[3].value, pTable[2].scale, pTable[3].scale);
  else if (value < pTable[4].value)
    interpValue = interpStep(value, pTable[3].value, pTable[4].value, pTable[3].scale, pTable[4].scale);
  else
    interpValue = pTable[4].scale;

  return interpValue;
}

static float scale(int32_t value, Scale_t *pTable)
{
  float scaledValue;
  uint32_t interpValue;

  interpValue = interp(value, pTable);
  scaledValue = ((float)(interpValue)) / 100.0;

  return scaledValue;
}

static float calculateDampedValue(float previousValue, float newValue, float dampingFactor)
{
  return (previousValue * dampingFactor) + (newValue * (1.0 - dampingFactor));
}


#define QCAL 0

static const float quadrantValueDamping = 0.95;

void CockpitQDC::processNodeResponse(char *pMessage)
{
  float engine0_throttle;
  float engine0_mixture;
  float engine0_prop;
  float engine1_throttle;
  float engine1_mixture;
  float engine1_prop;
  int32_t value;
  
  //  cout << "Quadrant response: " << pMessage << endl;
  if (fStringStartsWith(pMessage, "@QUADRANT:"))
  {
    pMessage += 10;   // move past "@QUADRANT:"
    pMessage = getValue(pMessage, &value);
    engine0_throttle = calculateDampedValue(pEngine0_ThrottlePropertyNode->getPropertyValueFloat(), scale(value, engine0_throttle_scale), quadrantValueDamping);
#if QCAL == 1
    if (logLevel > 1)
      cout << "QDC: trottle[0]: " << value << endl;
#endif
    pMessage = getValue(pMessage, &value);
    engine0_mixture = calculateDampedValue(pEngine0_MixturePropertyNode->getPropertyValueFloat(), scale(value, engine0_mixture_scale), quadrantValueDamping);
#if QCAL == 2
    if (logLevel > 1)
      cout << "QDC: mixture[0]: " << value << endl;
#endif
    pMessage = getValue(pMessage, &value);
    engine0_prop = calculateDampedValue(pEngine0_PropPropertyNode->getPropertyValueFloat(), scale(value, engine0_prop_scale), quadrantValueDamping);
#if QCAL == 3
    if (logLevel > 1)
      cout << "QDC: prop[0]: " << value << endl;
#endif
    pMessage = getValue(pMessage, &value);
    engine1_throttle = calculateDampedValue(pEngine1_ThrottlePropertyNode->getPropertyValueFloat(), scale(value, engine1_throttle_scale), quadrantValueDamping);
#if QCAL == 4
    if (logLevel > 1)
      cout << "QDC: trottle[1]: " << value << endl;
#endif
    pMessage = getValue(pMessage, &value);
    engine1_mixture = calculateDampedValue(pEngine1_MixturePropertyNode->getPropertyValueFloat(), scale(value, engine1_mixture_scale), quadrantValueDamping);
#if QCAL == 5
    if (logLevel > 1)
      cout << "QDC: mixture[1]: " << value << endl;
#endif
    pMessage = getValue(pMessage, &value);
    engine1_prop = calculateDampedValue(pEngine1_PropPropertyNode->getPropertyValueFloat(), scale(value, engine1_prop_scale), quadrantValueDamping);
#if QCAL == 6
    if (logLevel > 1)
      cout << "QDC: prop[1]: " << value << endl;
#endif

    if (logLevel > 2)
      cout << "Quandrant: " << engine0_throttle << ", " << engine0_mixture << ", " << engine0_prop << ", " << engine1_throttle << ", " << engine1_mixture << ", " << engine1_prop << endl;

    // Post the quadrant values.
#ifndef OSHKOSH
    pEngine0_ThrottlePropertyNode->setPropertyValue(engine0_throttle);
    pEngine0_MixturePropertyNode->setPropertyValue(engine0_mixture);
    pEngine0_PropPropertyNode->setPropertyValue(engine0_prop);
    pEngine1_ThrottlePropertyNode->setPropertyValue(engine1_throttle);
    pEngine1_MixturePropertyNode->setPropertyValue(engine1_mixture);
    pEngine1_PropPropertyNode->setPropertyValue(engine1_prop);
#else
    pEngine0_ThrottlePropertyNode->setPropertyValue(engine0_throttle);

    // Engine #1 throttle stuck... make engine #1 throttle track engine #0 throttle
    pEngine1_ThrottlePropertyNode->setPropertyValue(engine0_throttle);

    // Mixtures and prop not working... use fixed values
    pEngine0_MixturePropertyNode->setPropertyValue(0.75);
    pEngine1_MixturePropertyNode->setPropertyValue(0.75);

    pEngine1_PropPropertyNode->setPropertyValue(0.95);
    pEngine0_PropPropertyNode->setPropertyValue(0.95);
#endif
  }
}

// End of File
