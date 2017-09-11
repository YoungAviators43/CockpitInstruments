/// File: CockpitSuctionGauge.cxx
///
/// Cockpit Instrument, Suction Gauge

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitSuctionGauge.hxx"

extern unsigned int logLevel;

class CockpitSuctionGauge : public CockpitNode
{
private:
  float currentSuctionInHg;
  PropertyNode *pSuctionPropertyNode;

public:
  CockpitSuctionGauge(CockpitNodeComm_t *pCommChannel);
  ~CockpitSuctionGauge();
  const char *getCockpitNodeInfo(void);
  void processNotification(void);
  const char *nodeTypeCode(void) { return "SUI"; };
  const char *nodeFullName(void) { return "Suction Guage"; };
  void fetchNodeMessage(char *buffer);
};

CockpitNode *createCockpitSuctionGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitSuctionGauge(pCommChannel);
}

const char *suctionInHgPropertyName = "/systems/vacuum/suction-inhg";

CockpitSuctionGauge::CockpitSuctionGauge(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "SuctionGauge")
{
  if (logLevel > 1)
    cout << "Creating CockpitSuctionGauge instrument." << endl;

  pSuctionPropertyNode = registerCockpitNodeProperty(suctionInHgPropertyName, propFloat);
  currentSuctionInHg = pSuctionPropertyNode->getPropertyValueFloat();
}

CockpitSuctionGauge::~CockpitSuctionGauge()
{
  propertyNodeUnregister(pSuctionPropertyNode);
}

const char *CockpitSuctionGauge::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.2f InHg", currentSuctionInHg);

  return infoStr;
}

void CockpitSuctionGauge::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetSUC:%1.0f", currentSuctionInHg * 100);
}

void CockpitSuctionGauge::processNotification(void)
{
  float newSuctionInHg;

  // Fetch and process new values.
  newSuctionInHg = pSuctionPropertyNode->getPropertyValueFloat();

  if ((newSuctionInHg < 0.0) || (newSuctionInHg > 10.0))
    newSuctionInHg = 0.0;

  if (newSuctionInHg != currentSuctionInHg)
  {
    // Indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitSuctionGauge: Suction (InHg): " << currentSuctionInHg << " => " << newSuctionInHg << endl;

    currentSuctionInHg = newSuctionInHg;

    indicateUpdateRequired();
  }
}

/// End of File
