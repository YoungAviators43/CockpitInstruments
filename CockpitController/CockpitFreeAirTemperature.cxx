/// File: CockpitFreeAirTemperature.cxx
///
/// Cockpit Instrument, Free (outside) Air Temperature Gauge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitFreeAirTemperature.hxx"

extern unsigned int logLevel;

class CockpitFreeAirTemp : public CockpitNode
{
private:
  float currentFreeAirTemp;
  PropertyNode *pFreeAirTempPropertyNode;

public:
  CockpitFreeAirTemp(CockpitNodeComm_t *pCommChannel);
  ~CockpitFreeAirTemp();
  const char *getCockpitNodeInfo(void);
  void processNotification(void);
  const char *nodeTypeCode(void) { return "FAT"; };
  const char *nodeFullName(void) { return "Free Air Temperature Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createFreeAirTemperatureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFreeAirTemp(pCommChannel);
}

const char *freeAirTemperaturePropertyName = "/environment/temperature-degc";

CockpitFreeAirTemp::CockpitFreeAirTemp(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "FreeAirTemperatureGauge")
{
  if (logLevel > 1)
    cout << "Creating CockpitFreeAirTemp instrument." << endl;

  pFreeAirTempPropertyNode = registerCockpitNodeProperty(freeAirTemperaturePropertyName, propFloat);
  currentFreeAirTemp = pFreeAirTempPropertyNode->getPropertyValueFloat();
}

CockpitFreeAirTemp::~CockpitFreeAirTemp()
{
  propertyNodeUnregister(pFreeAirTempPropertyNode);
}

const char *CockpitFreeAirTemp::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.1f C.", currentFreeAirTemp);

  return infoStr;
}

void CockpitFreeAirTemp::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetFAT:%1.0f", currentFreeAirTemp);
}

void CockpitFreeAirTemp::processNotification(void)
{
  float newFreeAirTemp;

  // Fetch and process new values.
  newFreeAirTemp = pFreeAirTempPropertyNode->getPropertyValueFloat();
  if (newFreeAirTemp != currentFreeAirTemp)
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitFreeAirTemperature: Temperature (C): " << currentFreeAirTemp << " => " << newFreeAirTemp << endl;

    currentFreeAirTemp = newFreeAirTemp;

    indicateUpdateRequired();
  }
}

/// End of File
