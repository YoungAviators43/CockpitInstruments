/// File: CockpitMPI.cxx
///
/// Cockpit Instrument, Manifold Pressure Indicator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitMPI.hxx"

extern unsigned int logLevel;

class CockpitMPI : public CockpitNode
{
private:
  float engine0_mpi;
  float engine1_mpi;
  PropertyNode *pEngine0MpiPropertyNode;
  PropertyNode *pEngine1MpiPropertyNode;

public:
  CockpitMPI(CockpitNodeComm_t *pCommChannel);
  ~CockpitMPI();
  void processNotification(void);
  const char *nodeTypeCode(void) { return "MPI"; };
  const char *nodeFullName(void) { return "Manifold Pressure Indicator"; };
  void fetchNodeMessage(char *buffer);
  const char *getCockpitNodeInfo(void);
};

CockpitNode *createCockpitMPI(CockpitNodeComm_t *pCommChannel)
{
  return new CockpitMPI(pCommChannel);
}

static const char *engine0MpiPropertyName = "/engines/engine[0]/mp-inhg";
static const char *engine1MpiPropertyName = "/engines/engine[1]/mp-inhg";

CockpitMPI::CockpitMPI(CockpitNodeComm_t *pCommChannel) :
  CockpitNode(pCommChannel, "MpiInstrument")
{
  pEngine0MpiPropertyNode = registerCockpitNodeProperty(engine0MpiPropertyName, propFloat);
  pEngine1MpiPropertyNode = registerCockpitNodeProperty(engine1MpiPropertyName, propFloat);

  engine0_mpi = pEngine0MpiPropertyNode->getPropertyValueFloat();
  engine1_mpi = pEngine1MpiPropertyNode->getPropertyValueFloat();
}

CockpitMPI::~CockpitMPI()
{
  propertyNodeUnregister(pEngine0MpiPropertyNode);
  propertyNodeUnregister(pEngine1MpiPropertyNode);
}

const char *CockpitMPI::getCockpitNodeInfo(void)
{
  sprintf(infoStr, "%5.0f psi, %5.0f psi", engine0_mpi, engine1_mpi);

  return infoStr;
}

void CockpitMPI::fetchNodeMessage(char *buffer)
{
  sprintf(buffer, "!SetMPI:%1.0f,%1.0f", engine0_mpi, engine1_mpi);
}

void CockpitMPI::processNotification(void)
{
  float newEngine0Mpi;
  float newEngine1Mpi;

  // Fetch and process new values.
  newEngine0Mpi = pEngine0MpiPropertyNode->getPropertyValueFloat();
  newEngine1Mpi = pEngine1MpiPropertyNode->getPropertyValueFloat();
  if ((newEngine0Mpi != engine0_mpi) || (newEngine1Mpi != engine1_mpi))
  {
    // indicate that an message needs to be sent to the instrument
    if (logLevel > 1)
      cout << "CockpitMPI: engine 0: " << engine0_mpi << " => " << newEngine0Mpi
	   << ", engine 1: " << engine1_mpi << " => " << newEngine1Mpi << endl;
    engine0_mpi = newEngine0Mpi;
    engine1_mpi = newEngine1Mpi;

    indicateUpdateRequired();
  }
}

/// End of File
