/// File: Property.cxx
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "CockpitNodeInternal.hxx"


void notifyCockpitNode(CockpitNode *pCockpitNode)
{
  pCockpitNode->NotifyModified();
}

const char *cockpitNodeName(CockpitNode *pCockpitNode)
{
  return pCockpitNode->getCockpitNodeName();
}

void processCockpitNodeWrite(CockpitNode *pNode)
{
}

void processCockpitNodeRead(CockpitNode *pNode)
{
}
