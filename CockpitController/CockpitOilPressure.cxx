/// File: CockpitOilPres.cxx
///
/// Cockpit Instrument, Engine Oil Pressure indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitOilPressure.hxx"

#define EMULATE_OIL_PRESSURE

extern unsigned int logLevel;

#ifndef EMULATE_OIL_PRESSURE

class CockpitOilPressure : public CockpitNode
{
private:
  float engine0_OilPressure;
  float engine1_OilPressure;
  PropertyNode *pEngine0OilPressurePropertyNode;
  PropertyNode *pEngine1OilPressurePropertyNode;

public:
  CockpitOilPressure(CockpitNodeComm_t *pCommChannel);
  ~CockpitOilPressure();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "OPI"; };
  const char *nodeFullName(void) { return "Oil Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitOilPressureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitOilPressure(pCommChannel);
}

static const char *engine0OilPressurePropertyName = "/engines/engine[0]/oil-pressure-psi";
static const char *engine1OilPressurePropertyName = "/engines/engine[1]/oil-pressure-psi";

CockpitOilPressure::CockpitOilPressure(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "OilPressureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitOilPressure instrument." << endl;

  pEngine0OilPressurePropertyNode = registerCockpitNodeProperty(engine0OilPressurePropertyName, propFloat);
  pEngine1OilPressurePropertyNode = registerCockpitNodeProperty(engine1OilPressurePropertyName, propFloat);

  engine0_OilPressure = pEngine0OilPressurePropertyNode->getPropertyValueFloat();
  engine1_OilPressure = pEngine1OilPressurePropertyNode->getPropertyValueFloat();
}

CockpitOilPressure::~CockpitOilPressure()
{
  propertyNodeUnregister(pEngine0OilPressurePropertyNode);
  propertyNodeUnregister(pEngine1OilPressurePropertyNode);
}

const char *CockpitOilPressure::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f psi, %5.0f psi", engine0_OilPresure, engine1_OilPressure);

  return infoStr;
}

void CockpitOilPressure::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetOPR:%1.0f,%1.0f", engine0_OilPressure, engine1_OilPressure);
}

void CockpitOilPressure::processNotification(void)
{
  float newEngine0_OilPressure;
  float newEngine1_OilPressure;

  // Fetch and process new values.
  newEngine0_OilPressure = pEngine0OilPressurePropertyNode->getPropertyValueFloat();
  newEngine1_OilPressure = pEngine1OilPressurePropertyNode->getPropertyValueFloat();
  if ((newEngine0_OilPressure != engine0_OilPressure) ||
      (newEngine1_OilPressure != engine1_OilPressure))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitOilPressure: engine 0: " << engine0_OilPressure << " => " << newEngine0_OilPressure
	   << ", engine 1: " << engine1_OilPressure << " => " << newEngine1_OilPressure << endl;

    engine0_OilPressure = newEngine0_OilPressure;
    engine1_OilPressure = newEngine1_OilPressure;

    indicateUpdateRequired();
  }
}

#else // emulation

/// Emulate oil pressure by monitoring engine rpm and calculating it.

class CockpitOilPressure : public CockpitNode
{
private:
  float engine0_OilPressure;
  float engine1_OilPressure;

  PropertyNode *pEngine0RpmPropertyNode;
  PropertyNode *pEngine1RpmPropertyNode;

  float oilPressureFromRpm(float rpm);
  float calcDampedValue(float oldValue, float newValue, float damping);

public:
  CockpitOilPressure(CockpitNodeComm_t *pCommChannel);
  ~CockpitOilPressure();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "OPI"; };
  const char *nodeFullName(void) { return "Oil Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitOilPressureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitOilPressure(pCommChannel);
}

static const char *engine0RpmPropertyName = "/engines/engine[0]/rpm";
static const char *engine1RpmPropertyName = "/engines/engine[1]/rpm";

CockpitOilPressure::CockpitOilPressure(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "OilPressureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitOilPressure instrument." << endl;

  pEngine0RpmPropertyNode = registerCockpitNodeProperty(engine0RpmPropertyName, propFloat);
  pEngine1RpmPropertyNode = registerCockpitNodeProperty(engine1RpmPropertyName, propFloat);

  engine0_OilPressure = oilPressureFromRpm(pEngine0RpmPropertyNode->getPropertyValueFloat());
  engine1_OilPressure = oilPressureFromRpm(pEngine1RpmPropertyNode->getPropertyValueFloat());
}

CockpitOilPressure::~CockpitOilPressure()
{
  propertyNodeUnregister(pEngine0RpmPropertyNode);
  propertyNodeUnregister(pEngine1RpmPropertyNode);
}

const char *CockpitOilPressure::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f psi, %5.0f psi", engine0_OilPressure, engine1_OilPressure);

  return infoStr;
}

void CockpitOilPressure::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetOPR:%1.0f,%1.0f", engine0_OilPressure, engine1_OilPressure);
}

float CockpitOilPressure::calcDampedValue(float oldValue, float newValue, float damping)
{
  return (oldValue * damping) + (newValue * (1.0 - damping));
}


float CockpitOilPressure::oilPressureFromRpm(float rpm)
{
  float oilPressure;

  if (rpm > 1000.0)
  {
    // Greater than 1000 rpm use fixed "green" bar value.
    oilPressure = 85.0;
  }
  else if (rpm > 500.0)
  {
    // Between 500 and 1000 rpm use fixed (but damped value)
    oilPressure = 70.0;
  }
  else if (rpm > 10.0)
  {
    // Between 10 and 500 rpm use ramp.
    oilPressure = rpm * 70.0/500.0;
  }
  else
  {
    oilPressure = 0.0;
  }
  return oilPressure;
}


static const float rpmToOilPressureDamping = 0.95;

void CockpitOilPressure::processNotification(void)
{
  float newEngine0_OilPressure;
  float newEngine1_OilPressure;

  // Fetch and process new values.
  newEngine0_OilPressure = calcDampedValue(engine0_OilPressure, oilPressureFromRpm(pEngine0RpmPropertyNode->getPropertyValueFloat()), rpmToOilPressureDamping);
  newEngine1_OilPressure = calcDampedValue(engine1_OilPressure, oilPressureFromRpm(pEngine1RpmPropertyNode->getPropertyValueFloat()), rpmToOilPressureDamping);

  if ((newEngine0_OilPressure != engine0_OilPressure) ||
      (newEngine1_OilPressure != engine1_OilPressure))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitOilPressure: engine 0: " << engine0_OilPressure << " => " << newEngine0_OilPressure
	   << ", engine 1: " << engine1_OilPressure << " => " << newEngine1_OilPressure << endl;

    engine0_OilPressure = newEngine0_OilPressure;
    engine1_OilPressure = newEngine1_OilPressure;

    indicateUpdateRequired();
  }
}

#endif

/// End of File
