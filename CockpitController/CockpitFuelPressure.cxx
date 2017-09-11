/// File: CockpitFuelPressure.cxx
///
/// Cockpit Instrument, Engine Fuel Pressure indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitFuelPressure.hxx"

extern unsigned int logLevel;

#define EMULATE_FUEL_PRESSURE

#ifndef EMULATE_FUEL_PRESSURE

class CockpitFuelPressure : public CockpitNode
{
private:
  float engine0_FuelPressure;
  float engine1_FuelPressure;

  PropertyNode *pEngine0FuelPressurePropertyNode;
  PropertyNode *pEngine1FuelPressurePropertyNode;

public:
  CockpitFuelPressure(CockpitNodeComm_t *pCommChannel);
  ~CockpitFuelPressure();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "FPI"; };
  const char *nodeFullName(void) { return "Fuel Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitFuelPressureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFuelPressure(pCommChannel);
}

static const char *engine0FuelPressurePropertyName = "/engines/engine[0]/fuel-psi-norm";
static const char *engine1FuelPressurePropertyName = "/engines/engine[1]/fuel-psi-norm";

CockpitFuelPressure::CockpitFuelPressure(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "FuelPressureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitFuelPressure instrument." << endl;

  pEngine0FuelPressurePropertyNode = registerCockpitNodeProperty(engine0FuelPressurePropertyName, propFloat);
  pEngine1FuelPressurePropertyNode = registerCockpitNodeProperty(engine1FuelPressurePropertyName, propFloat);

  engine0_FuelPressure = pEngine0FuelPressurePropertyNode->getPropertyValueFloat();
  engine1_FuelPressure = pEngine1FuelPressurePropertyNode->getPropertyValueFloat();
}

CockpitFuelPressure::~CockpitFuelPressure()
{
  propertyNodeUnregister(pEngine0FuelPressurePropertyNode);
  propertyNodeUnregister(pEngine1FuelPressurePropertyNode);
}

void CockpitFuelPressure::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetFPI:%1.0f,%1.0f", engine0_FuelPressure, engine1_FuelPressure);
}

void CockpitFuelPressure::processNotification(void)
{
  float newEngine0_FuelPressure;
  float newEngine1_FuelPressure;

  // Fetch and process new values.
  newEngine0_FuelPressure = pEngine0FuelPressurePropertyNode->getPropertyValueFloat();
  newEngine1_FuelPressure = pEngine1FuelPressurePropertyNode->getPropertyValueFloat();
  if ((newEngine0_FuelPressure != engine0_FuelPressure) ||
      (newEngine1_FuelPressure != engine1_FuelPressure))
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitFuelPressure: engine 0: " << engine0_FuelPressure << " => " << newEngine0_FuelPressure
	   << ", engine 1: " << engine1_FuelPressure << " => " << newEngine1_FuelPressure << endl;

    engine0_FuelPressure = newEngine0_FuelPressure;
    engine1_FuelPressure = newEngine1_FuelPressure;

    indicateUpdateRequired();
  }
}

#else
/// Emulate Fuel Pressure since FlightGear is missing this.

class CockpitFuelPressure : public CockpitNode
{
private:
  float engine0_FuelPressure;
  float engine1_FuelPressure;

  PropertyNode *pEngine0RpmPropertyNode;
  PropertyNode *pEngine1RpmPropertyNode;

  float fuelPressureFromRpm(float rpm);
  float calcDampedValue(float oldValue, float newValue, float damping);

public:
  CockpitFuelPressure(CockpitNodeComm_t *pCommChannel);
  ~CockpitFuelPressure();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "FPI"; };
  const char *nodeFullName(void) { return "Fuel Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitFuelPressureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFuelPressure(pCommChannel);
}

static const char *engine0RpmPropertyName = "/engines/engine[0]/rpm";
static const char *engine1RpmPropertyName = "/engines/engine[1]/rpm";

CockpitFuelPressure::CockpitFuelPressure(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "FuelPressureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitFuelPressure instrument." << endl;

  pEngine0RpmPropertyNode = registerCockpitNodeProperty(engine0RpmPropertyName, propFloat);
  pEngine1RpmPropertyNode = registerCockpitNodeProperty(engine1RpmPropertyName, propFloat);

  engine0_FuelPressure = fuelPressureFromRpm(pEngine0RpmPropertyNode->getPropertyValueFloat());
  engine1_FuelPressure = fuelPressureFromRpm(pEngine1RpmPropertyNode->getPropertyValueFloat());
}

CockpitFuelPressure::~CockpitFuelPressure()
{
  propertyNodeUnregister(pEngine0RpmPropertyNode);
  propertyNodeUnregister(pEngine1RpmPropertyNode);
}

const char *CockpitFuelPressure::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f psi, %5.0f psi", engine0_FuelPressure, engine1_FuelPressure);

  return infoStr;
}

void CockpitFuelPressure::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetFPI:%1.0f,%1.0f", engine0_FuelPressure, engine1_FuelPressure);
}

float CockpitFuelPressure::calcDampedValue(float oldValue, float newValue, float damping)
{
  return (oldValue * damping) + (newValue * (1.0 - damping));
}


static const float FuelPressureNominal = 22.0;  // psi

float CockpitFuelPressure::fuelPressureFromRpm(float rpm)
{
  float fuelPressure;

  if (rpm > 100.0)
  {
    // Above 100 rpm used fixed "green" value.
    fuelPressure = FuelPressureNominal;
  }
  else if (rpm > 10.0)
  {
    // Between 10 and 100 rpm use ramp.
    fuelPressure = rpm * FuelPressureNominal / 100.0;
  }
  else
  {
    fuelPressure = 0.0;
  }
  return fuelPressure;
}


static const float rpmToFuelPressureDamping = 0.90;

void CockpitFuelPressure::processNotification(void)
{
  float newEngine0_FuelPressure;
  float newEngine1_FuelPressure;

  // Fetch and process new values.
  newEngine0_FuelPressure = calcDampedValue(engine0_FuelPressure, fuelPressureFromRpm(pEngine0RpmPropertyNode->getPropertyValueFloat()), rpmToFuelPressureDamping);
  newEngine1_FuelPressure = calcDampedValue(engine1_FuelPressure, fuelPressureFromRpm(pEngine1RpmPropertyNode->getPropertyValueFloat()), rpmToFuelPressureDamping);

  if ((newEngine0_FuelPressure != engine0_FuelPressure) ||
      (newEngine1_FuelPressure != engine1_FuelPressure))
  {
    // Indicate that a message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitFuelPressure: engine 0: " << engine0_FuelPressure << " => " << newEngine0_FuelPressure
	   << ", engine 1: " << engine1_FuelPressure << " => " << newEngine1_FuelPressure << endl;

    engine0_FuelPressure = newEngine0_FuelPressure;
    engine1_FuelPressure = newEngine1_FuelPressure;

    indicateUpdateRequired();
  }
}

#endif

/// End of File
