/// File: CockpitASI.cxx
///
/// Cockpit Instrument, Airspeed Speed Indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitPanel.hxx"
#include "CockpitASI.hxx"

extern unsigned int logLevel;

class CockpitASI : public CockpitNode
{
private:
  float currentAirspeedMph;
  PropertyNode *pAsiPropertyNode;

public:
  CockpitASI(CockpitNodeComm_t *pCommNode);
  ~CockpitASI();
  void processNotification(void);
  void fetchNodeMessage(char *buffer);
  const char *nodeTypeCode(void) { return "ASI"; };
  const char *nodeFullName(void) { return "Airspeed Indicator"; };
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitASI(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitASI(pCommChannel);
}

static const char *asiPropertyName = "/instrumentation/airspeed-indicator/indicated-speed-mph";

CockpitASI::CockpitASI(CockpitNodeComm_t *pCommNode) : CockpitNode(pCommNode, "AsiInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitASI instrument." << endl;

  pAsiPropertyNode = registerCockpitNodeProperty(asiPropertyName, propFloat);
  currentAirspeedMph = pAsiPropertyNode->getPropertyValueFloat();
}

CockpitASI::~CockpitASI()
{
  propertyNodeUnregister(pAsiPropertyNode);
}

const char *CockpitASI::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f mph", currentAirspeedMph);

  return infoStr;
};

void CockpitASI::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetIAS:%1.0f", currentAirspeedMph);
}

void CockpitASI::processNotification(void)
{
  float newAirspeedMph;

  // Fetch and process new values.
  newAirspeedMph = pAsiPropertyNode->getPropertyValueFloat();
  if (newAirspeedMph != currentAirspeedMph)
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitASI: air speed: " << currentAirspeedMph << " => " << newAirspeedMph << endl;

    currentAirspeedMph = newAirspeedMph;
    indicateUpdateRequired();
  }
}

/// End of File
