/// File: CockpitVSI.cxx
///
/// Cockpit Instrument, Vertical Speed Indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitVSI.hxx"

extern unsigned int logLevel;

class CockpitVSI : public CockpitNode
{
private:
  float currentVerticalSpeed;
  PropertyNode *pVsiPropertyNode;

public:
  CockpitVSI(CockpitNodeComm_t *pCommChannel);
  ~CockpitVSI();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "VSI"; };
  const char *nodeFullName(void) { return "Vertical Speed Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitVSI(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitVSI(pCommChannel);
}

static const char *vsiPropertyName = "/instrumentation/vertical-speed-indicator/indicated-speed-fpm";

CockpitVSI::CockpitVSI(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "VsiInstrument")
{
  if (logLevel > 1)
    cout << "Creating CockpitVSI instrument." << endl;
  pVsiPropertyNode = registerCockpitNodeProperty(vsiPropertyName, propFloat);

  currentVerticalSpeed = pVsiPropertyNode->getPropertyValueFloat();
}

CockpitVSI::~CockpitVSI()
{
  propertyNodeUnregister(pVsiPropertyNode);
}

const char *CockpitVSI::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.2f hfm", currentVerticalSpeed / 100.0);

  return infoStr;
}

void CockpitVSI::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetVSI:%1.0f", currentVerticalSpeed);
}

void CockpitVSI::processNotification(void)
{
  float newVerticalSpeed;

  // Fetch and process new values.
  newVerticalSpeed = pVsiPropertyNode->getPropertyValueFloat();
  if (newVerticalSpeed != currentVerticalSpeed)
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitVSI: vertical speed: " << currentVerticalSpeed << " => " << newVerticalSpeed << endl;
    currentVerticalSpeed = newVerticalSpeed;

    indicateUpdateRequired();
  }
}

/// End of File
