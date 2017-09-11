/// File: CockpitCarbTemperature.cxx
///
/// Cockpit Instrument, Engine Carb Temperature indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitCarbTemperature.hxx"

extern unsigned int logLevel;

class CockpitCarbTemperature : public CockpitNode
{
private:
  float engine0_CarbTemperature;
  float engine1_CarbTemperature;
  PropertyNode *pEngine0CarbTemperaturePropertyNode;
  PropertyNode *pEngine1CarbTemperaturePropertyNode;

public:
  CockpitCarbTemperature(CockpitNodeComm_t *pCommChannel);
  ~CockpitCarbTemperature();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "CAI"; };
  const char *nodeFullName(void) { return "Carburetor Air Temperature Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitCarbTemperatureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitCarbTemperature(pCommChannel);
}


static const char *engine0CarbTemperaturePropertyName = "/engines/engine[0]/carb-temp-degc";
static const char *engine1CarbTemperaturePropertyName = "/engines/engine[1]/carb-temp-degc";

CockpitCarbTemperature::CockpitCarbTemperature(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "CarbTemperatureInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitCarbTemperature instrument." << endl;

  pEngine0CarbTemperaturePropertyNode = registerCockpitNodeProperty(engine0CarbTemperaturePropertyName, propFloat);
  pEngine1CarbTemperaturePropertyNode = registerCockpitNodeProperty(engine1CarbTemperaturePropertyName, propFloat);

  engine0_CarbTemperature = pEngine0CarbTemperaturePropertyNode->getPropertyValueFloat();
  engine1_CarbTemperature = pEngine1CarbTemperaturePropertyNode->getPropertyValueFloat();
}

CockpitCarbTemperature::~CockpitCarbTemperature()
{
  propertyNodeUnregister(pEngine0CarbTemperaturePropertyNode);
  propertyNodeUnregister(pEngine1CarbTemperaturePropertyNode);
}

const char *CockpitCarbTemperature::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f C, %5.0f C", engine0_CarbTemperature, engine1_CarbTemperature);

  return infoStr;
}

void CockpitCarbTemperature::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetCAT:%1.0f,%1.0f", engine0_CarbTemperature, engine1_CarbTemperature);
}

void CockpitCarbTemperature::processNotification(void)
{
  float newEngine0_CarbTemperature;
  float newEngine1_CarbTemperature;

  // Fetch and process new values.
  newEngine0_CarbTemperature = pEngine0CarbTemperaturePropertyNode->getPropertyValueFloat();
  newEngine1_CarbTemperature = pEngine1CarbTemperaturePropertyNode->getPropertyValueFloat();
  if ((newEngine0_CarbTemperature != engine0_CarbTemperature) ||
      (newEngine1_CarbTemperature != engine1_CarbTemperature))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitCarbTemperature: engine 0: " << engine0_CarbTemperature << " => " << newEngine0_CarbTemperature
	   << ", engine 1: " << engine1_CarbTemperature << " => " << newEngine1_CarbTemperature << endl;

    engine0_CarbTemperature = newEngine0_CarbTemperature;
    engine1_CarbTemperature = newEngine1_CarbTemperature;

    indicateUpdateRequired();
  }
}

/// End of File
