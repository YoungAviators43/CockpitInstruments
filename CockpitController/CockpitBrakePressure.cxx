/// File: CockpitBrakePressure.cxx
///
/// Cockpit Instrument, Brake Pressure Gauge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitBrakePressure.hxx"

extern unsigned int logLevel;

class CockpitBrakePressure : public CockpitNode
{
private:
  float currentBrakePressure;
  PropertyNode *pBrakePressurePropertyNode;

public:
  CockpitBrakePressure(CockpitNodeComm_t *pCommChannel);
  ~CockpitBrakePressure();
  const char *getCockpitNodeInfo(void);
  void processNotification(void);
  const char *nodeTypeCode(void) { return "BPI"; };
  const char *nodeFullName(void) { return "Brake Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitBrakePressureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitBrakePressure(pCommChannel);
}

static const char *brakePressurePropertyName = "/systems/hydraulic/brakes-psi";

CockpitBrakePressure::CockpitBrakePressure(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "BrakePressureGauge")
{
  if (logLevel > 1)
    cout << "Creating CockpitBrakePressure instrument." << endl;

  pBrakePressurePropertyNode = registerCockpitNodeProperty(brakePressurePropertyName, propFloat);
  currentBrakePressure = pBrakePressurePropertyNode->getPropertyValueFloat();
}

CockpitBrakePressure::~CockpitBrakePressure()
{
  propertyNodeUnregister(pBrakePressurePropertyNode);
}

const char *CockpitBrakePressure::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.0f PSI", currentBrakePressure);

  return infoStr;
}

void CockpitBrakePressure::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetBPI:%1.0f", currentBrakePressure);
}

void CockpitBrakePressure::processNotification(void)
{
  float newBrakePressure;

  // Fetch and process new values.
  newBrakePressure = pBrakePressurePropertyNode->getPropertyValueFloat();
  if (newBrakePressure != currentBrakePressure)
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitBrakePressure: Pressure (PSI): "
	   << currentBrakePressure << " => " << newBrakePressure << endl;

    currentBrakePressure = newBrakePressure;

    indicateUpdateRequired();
  }
}

/// End of File
