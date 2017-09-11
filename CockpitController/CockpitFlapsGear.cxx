/// File: CockpitFlapsGear.cxx
///
/// Cockpit Instrument, Flaps and Gear Position Gauge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitFlapsGear.hxx"

extern unsigned int logLevel;

class CockpitFlapsGear : public CockpitNode
{
private:
  float currentFlapsPosition;
  float currentGear0Position;
  float currentGear1Position;
  float currentGear2Position;

  PropertyNode *pFlapsPropertyNode;
  PropertyNode *pGear0PropertyNode;
  PropertyNode *pGear1PropertyNode;
  PropertyNode *pGear2PropertyNode;

public:
  CockpitFlapsGear(CockpitNodeComm_t *pCommChannel);
  ~CockpitFlapsGear();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "FGI"; };
  const char *nodeFullName(void) { return "Flaps/Gear Position Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitFlapsGearGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFlapsGear(pCommChannel);
}

static const char *flapsPositionPropertyName = "/surface-positions/flap-pos-norm";
static const char *gear0PositionPropertyName = "/gear/gear[0]/position-norm";
static const char *gear1PositionPropertyName = "/gear/gear[1]/position-norm";
static const char *gear2PositionPropertyName = "/gear/gear[2]/position-norm";


CockpitFlapsGear::CockpitFlapsGear(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "FlapsGearPositionGauge"),
  currentFlapsPosition(0.0),
  currentGear0Position(0.0),
  currentGear1Position(0.0),
  currentGear2Position(0.0)
{
  if (logLevel > 1)
    cout << "Creating CockpitFlapsGear Position instrument." << endl;

  pFlapsPropertyNode = registerCockpitNodeProperty(flapsPositionPropertyName, propFloat);
  pGear0PropertyNode = registerCockpitNodeProperty(gear0PositionPropertyName, propFloat);
  pGear1PropertyNode = registerCockpitNodeProperty(gear1PositionPropertyName, propFloat);
  pGear2PropertyNode = registerCockpitNodeProperty(gear2PositionPropertyName, propFloat);
}

CockpitFlapsGear::~CockpitFlapsGear()
{
  propertyNodeUnregister(pFlapsPropertyNode);
  propertyNodeUnregister(pGear0PropertyNode);
  propertyNodeUnregister(pGear1PropertyNode);
  propertyNodeUnregister(pGear2PropertyNode);
}

void CockpitFlapsGear::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "SetFLG:%4.0f,%4.0f,%4.0f,%4.0f",
	  currentFlapsPosition,
	  currentGear0Position,
	  currentGear1Position,
	  currentGear2Position);
}

void CockpitFlapsGear::processNotification(void)
{
  float newFlapsPosition;
  float newGear0Position;
  float newGear1Position;
  float newGear2Position;

  // Fetch and process new values.
  newFlapsPosition = pFlapsPropertyNode->getPropertyValueFloat();
  newGear0Position = pGear0PropertyNode->getPropertyValueFloat();
  newGear1Position = pGear1PropertyNode->getPropertyValueFloat();
  newGear2Position = pGear2PropertyNode->getPropertyValueFloat();
  if (newFlapsPosition != currentFlapsPosition ||
      newGear0Position != currentGear0Position ||
      newGear1Position != currentGear1Position ||
      newGear2Position != currentGear2Position)
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitFlapsGear: Position:"
	   << " Flaps: " << currentFlapsPosition << " => " << newFlapsPosition
	   << " Gear0: " << currentGear0Position << " => " << newGear0Position
	   << " Gear1: " << currentGear1Position << " => " << newGear1Position
	   << " Gear2: " << currentGear2Position << " => " << newGear2Position
	   << endl;

    currentFlapsPosition = newFlapsPosition;
    currentGear0Position = newGear0Position;
    currentGear1Position = newGear1Position;
    currentGear2Position = newGear2Position;

    indicateUpdateRequired();
  }
}

/// End of File
