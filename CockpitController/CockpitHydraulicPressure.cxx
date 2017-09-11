/// File: CockpitHydraulicPressure.cxx
///
/// Cockpit Instrument, Hydraulic Pressure Gauge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitHydraulicPressure.hxx"

extern unsigned int logLevel;

class CockpitHydraulicPressure : public CockpitNode
{
private:
  float currentHydraulicPressure;
  PropertyNode *pHydraulicPressurePropertyNode;

public:
  CockpitHydraulicPressure(CockpitNodeComm_t *pCommChannel);
  ~CockpitHydraulicPressure();
  const char *getCockpitNodeInfo(void);
  void processNotification(void);
  const char *nodeTypeCode(void) { return "HPI"; };
  const char *nodeFullName(void) { return "Hydraulic Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitHydraulicPressureGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitHydraulicPressure(pCommChannel);
}

static const char *hydraulicPressurePropertyName = "/systems/hydraulics/pump-psi";

CockpitHydraulicPressure::CockpitHydraulicPressure(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "HydraulicPressureGauge")
{
  if (logLevel > 1)
    cout << "Creating CockpitHydraulicPressure instrument." << endl;

  pHydraulicPressurePropertyNode = registerCockpitNodeProperty(hydraulicPressurePropertyName, propFloat);
  currentHydraulicPressure = pHydraulicPressurePropertyNode->getPropertyValueFloat();
}

CockpitHydraulicPressure::~CockpitHydraulicPressure()
{
  propertyNodeUnregister(pHydraulicPressurePropertyNode);
}

const char *CockpitHydraulicPressure::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.0f PSI", currentHydraulicPressure);

  return infoStr;
}

void CockpitHydraulicPressure::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetHYD:%1.0f", currentHydraulicPressure);
}

void CockpitHydraulicPressure::processNotification(void)
{
  float newHydraulicPressure;

  // Fetch and process new values.
  newHydraulicPressure = pHydraulicPressurePropertyNode->getPropertyValueFloat();
  if (newHydraulicPressure != currentHydraulicPressure)
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitHydraulicPressure: Pressure (PSI): "
	   << currentHydraulicPressure << " => " << newHydraulicPressure << endl;

    currentHydraulicPressure = newHydraulicPressure;

    indicateUpdateRequired();
  }
}

/// End of File
