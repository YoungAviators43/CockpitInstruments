/// File: CockpitFuelLevel.cxx
///
/// Cockpit Instrument, Engine Fuel Level indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitFuelLevel.hxx"

extern unsigned int logLevel;

class CockpitFuelLevel : public CockpitNode
{
private:
  const char *guageTypeCode;
  const char *guageName;
  float engine0_FuelLevel;
  float engine1_FuelLevel;
  PropertyNode *pEngine0FuelLevelPropertyNode;
  PropertyNode *pEngine1FuelLevelPropertyNode;

public:
  CockpitFuelLevel(CockpitNodeComm_t *pCommChannel, const char *name, const char *typeCode, const char *tank1, const char *tank2);
  ~CockpitFuelLevel();
  void processNotification(void);
  const char *nodeTypeCode(void) { return guageTypeCode; };
  const char *nodeFullName(void) { return guageName; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createFrontTankFuelLevelGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFuelLevel(pCommChannel,
			      "Front Fuel Tank Level",
			      "FLF",
			      "/consumables/fuel/tank[0]/level-gal_us",
			      "/consumables/fuel/tank[1]/level-gal_us");
}

CockpitNode *createRearTankFuelLevelGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFuelLevel(pCommChannel,
			      "RearFuel Tank Level",
			      "FLR",
			      "/consumables/fuel/tank[2]/level-gal_us",
			      "/consumables/fuel/tank[3]/level-gal_us");
}

CockpitNode *createAuxTankFuelLevelGauge(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitFuelLevel(pCommChannel,
			      "Aux Fuel Tank Level",
			      "FLA",
			      "/consumables/fuel/tank[4]/level-gal_us",
			      "/consumables/fuel/tank[5]/level-gal_us");
}

CockpitFuelLevel::CockpitFuelLevel(CockpitNodeComm_t *pCommChannel, const char *name, const char *typeCode,
				   const char *tank1, const char *tank2) :
  CockpitNode(pCommChannel, name), guageTypeCode(typeCode), guageName(name)
{
  if (logLevel > 1)
    cout << "Creating CockpitFuelLevel instrument (" << name << ")" << endl;

  pEngine0FuelLevelPropertyNode = registerCockpitNodeProperty(tank1, propFloat);
  pEngine1FuelLevelPropertyNode = registerCockpitNodeProperty(tank2, propFloat);

  engine0_FuelLevel = pEngine0FuelLevelPropertyNode->getPropertyValueFloat();
  engine1_FuelLevel = pEngine1FuelLevelPropertyNode->getPropertyValueFloat();
}

CockpitFuelLevel::~CockpitFuelLevel()
{
  propertyNodeUnregister(pEngine0FuelLevelPropertyNode);
  propertyNodeUnregister(pEngine1FuelLevelPropertyNode);
}

const char *CockpitFuelLevel::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%6.0f Gal., %6.0f Gal.", engine0_FuelLevel, engine1_FuelLevel);

  return infoStr;
}

void CockpitFuelLevel::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetFLV:%1.0f,%1.0f", engine0_FuelLevel, engine1_FuelLevel);
}

void CockpitFuelLevel::processNotification(void)
{
  float newEngine0_FuelLevel;
  float newEngine1_FuelLevel;

  // Fetch and process new values.
  newEngine0_FuelLevel = pEngine0FuelLevelPropertyNode->getPropertyValueFloat();
  newEngine1_FuelLevel = pEngine1FuelLevelPropertyNode->getPropertyValueFloat();
  if ((newEngine0_FuelLevel != engine0_FuelLevel) ||
      (newEngine1_FuelLevel != engine1_FuelLevel))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitFuelLevel: engine 0: " << engine0_FuelLevel << " => " << newEngine0_FuelLevel
	   << ", engine 1: " << engine1_FuelLevel << " => " << newEngine1_FuelLevel << endl;
    engine0_FuelLevel = newEngine0_FuelLevel;
    engine1_FuelLevel = newEngine1_FuelLevel;

    indicateUpdateRequired();
  }
}

/// End of File

