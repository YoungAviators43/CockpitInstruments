/// File: CockpitRPM.cxx
///
/// Cockpit Instrument, Engine Tachometer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitRPM.hxx"

extern unsigned int logLevel;

class CockpitRPM : public CockpitNode
{
private:
  float engine0_rpm;
  float engine1_rpm;
  PropertyNode *pEngine0RpmPropertyNode;
  PropertyNode *pEngine1RpmPropertyNode;

public:
  CockpitRPM(CockpitNodeComm_t *pCommChannel);
  ~CockpitRPM();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "RPM"; };
  const char *nodeFullName(void) { return "Engine Tachometer"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitTachometer(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitRPM(pCommChannel);
}

static const char *engine0RpmPropertyName = "/engines/engine[0]/rpm";
static const char *engine1RpmPropertyName = "/engines/engine[1]/rpm";

CockpitRPM::CockpitRPM(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "RpmInstrument")
{
  pEngine0RpmPropertyNode = registerCockpitNodeProperty(engine0RpmPropertyName, propFloat);
  pEngine1RpmPropertyNode = registerCockpitNodeProperty(engine1RpmPropertyName, propFloat);

  engine0_rpm = pEngine0RpmPropertyNode->getPropertyValueFloat();
  engine1_rpm = pEngine1RpmPropertyNode->getPropertyValueFloat();
}

CockpitRPM::~CockpitRPM()
{
  propertyNodeUnregister(pEngine0RpmPropertyNode);
  propertyNodeUnregister(pEngine1RpmPropertyNode);
}

const char *CockpitRPM::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.0f RPM, %6.0f RPM", engine0_rpm, engine1_rpm);

  return infoStr;
}

void CockpitRPM::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetRPM:%1.0f,%1.0f", engine0_rpm, engine1_rpm);
}

void CockpitRPM::processNotification(void)
{
  float newEngine0rpm;
  float newEngine1rpm;

  // Fetch and process new values.
  newEngine0rpm = pEngine0RpmPropertyNode->getPropertyValueFloat();
  newEngine1rpm = pEngine1RpmPropertyNode->getPropertyValueFloat();
  if ((newEngine0rpm != engine0_rpm) || (newEngine1rpm != engine1_rpm))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitRPM: engine 0: " << engine0_rpm << " => " << newEngine0rpm
	   << ", engine 1: " << engine1_rpm << " => " << newEngine1rpm << endl;
    engine0_rpm = newEngine0rpm;
    engine1_rpm = newEngine1rpm;

    indicateUpdateRequired();
  }
}

/// End of File
