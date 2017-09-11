/// File: CockpitOilTemperature.cxx
///
/// Cockpit Instrument, Engine Oil Temperature indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitOilTemperature.hxx"

extern unsigned int logLevel;

#define EMULATE_OIL_TEMPERATURE

#ifndef EMULATE_OIL_TEMPERATURE

class CockpitOilTemperature : public CockpitNode
{
private:
  float engine0_OilTemperature;
  float engine1_OilTemperature;
  PropertyNode *pEngine0OilTemperaturePropertyNode;
  PropertyNode *pEngine1OilTemperaturePropertyNode;

public:
  CockpitOilTemperature(CockpitNodeComm_t *pCommChannel);
  ~CockpitOilTemperature();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "OTI"; };
  const char *nodeFullName(void) { return "Oil Temperature Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitOilTemperatureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitOilTemperature(pCommChannel);
}

static const char *engine0OilTemperaturePropertyName = "/engines/engine[0]/oil-temperature-degc";
static const char *engine1OilTemperaturePropertyName = "/engines/engine[1]/oil-temperature-degc";

CockpitOilTemperature::CockpitOilTemperature(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "OilTemperatureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitOilTemperature instrument." << endl;

  pEngine0OilTemperaturePropertyNode = registerCockpitNodeProperty(engine0OilTemperaturePropertyName, propFloat);
  pEngine1OilTemperaturePropertyNode = registerCockpitNodeProperty(engine1OilTemperaturePropertyName, propFloat);

  engine0_OilTemperature = pEngine0OilTemperaturePropertyNode->getPropertyValueFloat();
  engine1_OilTemperature = pEngine1OilTemperaturePropertyNode->getPropertyValueFloat();
}

CockpitOilTemperature::~CockpitOilTemperature()
{
  propertyNodeUnregister(pEngine0OilTemperaturePropertyNode);
  propertyNodeUnregister(pEngine1OilTemperaturePropertyNode);
}

void CockpitOilTemperature::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetOTP:%1.0f,%1.0f", engine0_OilTemperature, engine1_OilTemperature);
}

void CockpitOilTemperature::processNotification(void)
{
  float newEngine0_OilTemperature;
  float newEngine1_OilTemperature;

  // Fetch and process new values.
  newEngine0_OilTemperature = pEngine0OilTemperaturePropertyNode->getPropertyValueFloat();
  newEngine1_OilTemperature = pEngine1OilTemperaturePropertyNode->getPropertyValueFloat();
  if ((newEngine0_OilTemperature != engine0_OilTemperature) ||
      (newEngine1_OilTemperature != engine1_OilTemperature))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitOilTemperature: engine 0: " << engine0_OilTemperature << " => " << newEngine0_OilTemperature
	   << ", engine 1: " << engine1_OilTemperature << " => " << newEngine1_OilTemperature << endl;

    engine0_OilTemperature = newEngine0_OilTemperature;
    engine1_OilTemperature = newEngine1_OilTemperature;

    indicateUpdateRequired();
  }
}

#else

/// Emulate the engine oil temperature using cylinder head temperature as a proxy.

class CockpitOilTemperature : public CockpitNode
{
private:
  float engine0_OilTemperature;
  float engine1_OilTemperature;

  PropertyNode *pEngine0CylTemperaturePropertyNode;
  PropertyNode *pEngine1CylTemperaturePropertyNode;

  float oilTemperatureFromCylinderTemperature(float rpm);
  float calcDampedValue(float oldValue, float newValue, float damping);

public:
  CockpitOilTemperature(CockpitNodeComm_t *pCommChannel);
  ~CockpitOilTemperature();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "OTI"; };
  const char *nodeFullName(void) { return "Oil Temperature Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitOilTemperatureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitOilTemperature(pCommChannel);
}

static const char *engine0CylTemperaturePropertyName = "/engines/engine[0]/cyl-temp";
static const char *engine1CylTemperaturePropertyName = "/engines/engine[1]/cyl-temp";

CockpitOilTemperature::CockpitOilTemperature(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "OilTemperatureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitOilTemperature instrument." << endl;

  pEngine0CylTemperaturePropertyNode = registerCockpitNodeProperty(engine0CylTemperaturePropertyName, propFloat);
  pEngine1CylTemperaturePropertyNode = registerCockpitNodeProperty(engine1CylTemperaturePropertyName, propFloat);

  engine0_OilTemperature = oilTemperatureFromCylinderTemperature(pEngine0CylTemperaturePropertyNode->getPropertyValueFloat());
  engine1_OilTemperature = oilTemperatureFromCylinderTemperature(pEngine0CylTemperaturePropertyNode->getPropertyValueFloat());
}

CockpitOilTemperature::~CockpitOilTemperature()
{
  propertyNodeUnregister(pEngine0CylTemperaturePropertyNode);
  propertyNodeUnregister(pEngine1CylTemperaturePropertyNode);
}

const char *CockpitOilTemperature::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f C, %5.0f C", engine0_OilTemperature, engine1_OilTemperature);

  return infoStr;
}

void CockpitOilTemperature::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetOTP:%1.0f,%1.0f", engine0_OilTemperature, engine1_OilTemperature);
}

float CockpitOilTemperature::calcDampedValue(float oldValue, float newValue, float damping)
{
  return (oldValue * damping) + (newValue * (1.0 - damping));
}

/// Oil temperature nominal: 50-85 C
/// Cylinder head temperature nominal: 100-205 C
/// oil temp = 50.0 + (85.0 - 50.0) / (205.0 - 100.0) * (cylinder head temp - 100.0);
float CockpitOilTemperature::oilTemperatureFromCylinderTemperature(float cylinderTemperature)
{
  float oilTemperature;

  oilTemperature = 50.0 + ((85.0 - 50.0) / (205.0 - 100.0)) * (cylinderTemperature - 100.0);

  if (oilTemperature < 28.0)
    oilTemperature = 28.0;
  else if (oilTemperature > 85.0)
    oilTemperature = 85.0;

  return oilTemperature;
}

static const float cylinderToOilTemperatureDamping = 0.97;

void CockpitOilTemperature::processNotification(void)
{
  float newEngine0_OilTemperature;
  float newEngine1_OilTemperature;

  // Fetch and process new values.
  newEngine0_OilTemperature = calcDampedValue(engine0_OilTemperature,
					      oilTemperatureFromCylinderTemperature(pEngine0CylTemperaturePropertyNode->getPropertyValueFloat()),
					      cylinderToOilTemperatureDamping);
  newEngine1_OilTemperature = calcDampedValue(engine1_OilTemperature,
					      oilTemperatureFromCylinderTemperature(pEngine1CylTemperaturePropertyNode->getPropertyValueFloat()),
					      cylinderToOilTemperatureDamping);

  if ((newEngine0_OilTemperature != engine0_OilTemperature) ||
      (newEngine1_OilTemperature != engine1_OilTemperature))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitOilTemperature: engine 0: " << engine0_OilTemperature << " => " << newEngine0_OilTemperature
	   << ", engine 1: " << engine1_OilTemperature << " => " << newEngine1_OilTemperature << endl;

    engine0_OilTemperature = newEngine0_OilTemperature;
    engine1_OilTemperature = newEngine1_OilTemperature;

    indicateUpdateRequired();
  }
}

#endif

/// End of File
