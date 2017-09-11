/// File: CockpitCylTemperature.cxx
///
/// Cockpit Instrument, Engine Cylinder Temperature indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitCylTemperature.hxx"

extern unsigned int logLevel;

class CockpitCylTemperature : public CockpitNode
{
private:
  float engine0_CylTemperature;
  float engine1_CylTemperature;
  PropertyNode *pEngine0CylTemperaturePropertyNode;
  PropertyNode *pEngine1CylTemperaturePropertyNode;

public:
  CockpitCylTemperature(CockpitNodeComm_t *pCommChannel);
  ~CockpitCylTemperature();
  const char *getCockpitNodeInfo(void);
  void processNotification(void);
  const char *nodeTypeCode(void) { return "CYI"; };
  const char *nodeFullName(void) { return "Cylinder Head Temperature Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitCylTemperatureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitCylTemperature(pCommChannel);
}

static const char *engine0CylTemperaturePropertyName = "/engines/engine[0]/cyl-temp";
static const char *engine1CylTemperaturePropertyName = "/engines/engine[1]/cyl-temp";

CockpitCylTemperature::CockpitCylTemperature(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "CylTemperatureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitCylTemperature instrument." << endl;

  pEngine0CylTemperaturePropertyNode = registerCockpitNodeProperty(engine0CylTemperaturePropertyName, propFloat);
  pEngine1CylTemperaturePropertyNode = registerCockpitNodeProperty(engine1CylTemperaturePropertyName, propFloat);

  engine0_CylTemperature = pEngine0CylTemperaturePropertyNode->getPropertyValueFloat();
  engine1_CylTemperature = pEngine1CylTemperaturePropertyNode->getPropertyValueFloat();
}

CockpitCylTemperature::~CockpitCylTemperature()
{
  propertyNodeUnregister(pEngine0CylTemperaturePropertyNode);
  propertyNodeUnregister(pEngine1CylTemperaturePropertyNode);
}

const char *CockpitCylTemperature::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f C, %5.0f C", engine0_CylTemperature, engine1_CylTemperature);

  return infoStr;
}

void CockpitCylTemperature::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetCYL:%1.0f,%1.0f", engine0_CylTemperature, engine1_CylTemperature);
}

void CockpitCylTemperature::processNotification(void)
{
  float newEngine0_CylTemperature;
  float newEngine1_CylTemperature;

  // Fetch and process new values.
  newEngine0_CylTemperature = pEngine0CylTemperaturePropertyNode->getPropertyValueFloat();
  newEngine1_CylTemperature = pEngine1CylTemperaturePropertyNode->getPropertyValueFloat();
  if ((newEngine0_CylTemperature != engine0_CylTemperature) ||
      (newEngine1_CylTemperature != engine1_CylTemperature))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitCylTemperature: engine 0: " << engine0_CylTemperature << " => " << newEngine0_CylTemperature
	   << ", engine 1: " << engine1_CylTemperature << " => " << newEngine1_CylTemperature << endl;

    engine0_CylTemperature = newEngine0_CylTemperature;
    engine1_CylTemperature = newEngine1_CylTemperature;

    indicateUpdateRequired();
  }
}

/// End of File
