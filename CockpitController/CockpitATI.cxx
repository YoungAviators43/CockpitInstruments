/// File: CockpitATI.cxx
///
/// Cockpit Instrument, Attitude Indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitATI.hxx"

extern unsigned int logLevel;

class CockpitATI : public CockpitNode
{
private:
  float currentRollDeg;
  float currentPitchDeg;
  PropertyNode *pRollPropertyNode;
  PropertyNode *pPitchPropertyNode;

public:
  CockpitATI(CockpitNodeComm_t *pCommChannel);
  ~CockpitATI();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "ATI"; };
  const char *nodeFullName(void) { return "Attitude Indicator"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitATI(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitATI(pCommChannel);
}

static const char *atiRollPropertyName  = "/orientation/roll-deg";
static const char *atiPitchPropertyName = "/orientation/pitch-deg";

CockpitATI::CockpitATI(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "AtiInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitATI instrument." << endl;

  pRollPropertyNode = registerCockpitNodeProperty(atiRollPropertyName, propFloat);
  pPitchPropertyNode = registerCockpitNodeProperty(atiPitchPropertyName, propFloat);

  currentRollDeg = pRollPropertyNode->getPropertyValueFloat();
  currentPitchDeg = pPitchPropertyNode->getPropertyValueFloat();
}

CockpitATI::~CockpitATI()
{
  propertyNodeUnregister(pRollPropertyNode);
  propertyNodeUnregister(pPitchPropertyNode);
}

void CockpitATI::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetATI:%1.0f,%1.0f", currentRollDeg, currentPitchDeg);
}

void CockpitATI::processNotification(void)
{
  float newRollDeg;
  float newPitchDeg;

  // Fetch and process new values.
  newRollDeg = pRollPropertyNode->getPropertyValueFloat();
  newPitchDeg = pPitchPropertyNode->getPropertyValueFloat();
  if ((newRollDeg != currentRollDeg) || (newPitchDeg != currentPitchDeg))
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitATI: roll: " << currentRollDeg << " => " << newRollDeg
	   << ", pitch: " << currentPitchDeg << " => " << newPitchDeg << endl;

    currentRollDeg = newRollDeg;
    currentPitchDeg = newPitchDeg;

    indicateUpdateRequired();
  }
}

/// End of File
