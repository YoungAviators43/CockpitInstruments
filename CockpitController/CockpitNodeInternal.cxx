
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "CockpitNodeInternal.hxx"

extern unsigned int logLevel;

CockpitNode::CockpitNode(CockpitNodeComm_t *pCommNode, const char *nodeName) :
  cockpitNodeName(nodeName), cockpitCommNode(pCommNode)
{
  if (logLevel > 1)
    cout << "Info: CommChannel[" << commChannelGetPortPath(pCommNode) << "]: Identified as " << nodeName << ", creating." << endl;
}

void CockpitNode::NotifyModified(void)
{ 
  processNotification();
}

void CockpitNode::indicateUpdateRequired(void)
{
  indicateNodeModified(cockpitCommNode);
}

void CockpitNode::indicatorPropertyNodeNotFound(const char *pPropertyName)
{
  if (logLevel > 1)
    cout << getCockpitNodeName() << ": Property not found: " << pPropertyName << endl;
}

char CockpitNode::isPropertyTypeCorrect(PropertyNode *pPropertyNode, PropertyType_t pType)
{
  char isTypeCorrect = pPropertyNode->getPropertyType() == pType;
  if (!isTypeCorrect)
  {
    if (logLevel > 1)
      cout << getCockpitNodeName() << ": Property type mismatch for " << pPropertyNode->getPropertyName() <<
	      ", type was: " << getPropertyTypeName(pPropertyNode->getPropertyType()) <<
	      ", expected: " << getPropertyTypeName(pType) << endl;
  }
  return isTypeCorrect;
}


PropertyNode *CockpitNode::acquireCockpitNodeProperty(const char *propertyName, PropertyType_t pType)
{
  PropertyNode *pPropertyNode;

  pPropertyNode = getPropertyNode(propertyName);
  if (pPropertyNode == NULL)
  {
    indicatorPropertyNodeNotFound(propertyName);
  }
  else if (!isPropertyTypeCorrect(pPropertyNode, pType))
  {
    pPropertyNode = NULL;
  }
  return pPropertyNode;
}

PropertyNode *CockpitNode::registerCockpitNodeProperty(const char *propertyName, PropertyType_t pType)
{
  PropertyNode *pPropertyNode;

  pPropertyNode = acquireCockpitNodeProperty(propertyName, pType);
  if (pPropertyNode != NULL)
  {
    pPropertyNode->addSubscriber(this);
  }
  return pPropertyNode;
}

void CockpitNode::propertyNodeUnregister(PropertyNode *pPropertyNode)
{
  pPropertyNode->removeSubscriber(this);
}

void CockpitNode::processNodeIdentify(char *pMessage)
{
  pMessage += strlen("@IDENTIFY:");
  if (fStringStartsWith(pMessage, nodeTypeCode()))
  {
    // announce node
    if (logLevel > 1)
      cout << "Info: Attach " << nodeFullName() << ": " << pMessage << endl;

    cockpitNodeResponseAck(getCockpitCommNode());
    indicateUpdateRequired();
  }
  else
  {
    if (logLevel > 1)
      cout << "Info: Attach error: type code does not match: \"" << nodeTypeCode() << "\", \"" << pMessage << "\"" << endl;

    // invalid type code, does not match node.
    cockpitNodeResponseErr(getCockpitCommNode());
  }
}

void CockpitNode::processNodeStatus(char *pMessage)
{
  const char *pTypeCode = nodeTypeCode();

  pMessage += strlen("@STATUS:");
  if (fStringStartsWith(pMessage, pTypeCode))
  {
    cockpitNodeResponseAck(getCockpitCommNode());

    // Check if the node was in reset state.
    pMessage += strlen(pTypeCode);
    if (fStringStartsWith(pMessage, ",RESET"))
    {
      indicateUpdateRequired();
    }
  }
  else
  {
    if (logLevel > 1)
      cout << "Info: Status error: type code does not match: \"" << nodeTypeCode() << "\", \"" << pMessage << "\"" << endl;

    // invalid type code, does not match node.
    cockpitNodeResponseErr(getCockpitCommNode());
  }
}


void CockpitNode::composeNodeMessage(char *buffer)
{
  fetchNodeMessage(buffer);

  if (logLevel > 1)
    cout << "Info: CommChannel[" << commChannelGetPortPath(cockpitCommNode) << ", " << getCockpitNodeName() << "]: Send " << buffer << endl;
}

void CockpitNode::processNodeAck(char *msg)
{
  if (logLevel > 1)
    cout << "Info: CommChannel[" << commChannelGetPortPath(cockpitCommNode) << ", " << getCockpitNodeName() << "]: " << msg << endl;

  cockpitNodeResponseAck(getCockpitCommNode());
}

void CockpitNode::processNodeNak(char *msg)
{
  if (logLevel > 1)
    cout << "Info: CommChannel[" << commChannelGetPortPath(cockpitCommNode) << ", " << getCockpitNodeName() << "]: " << msg << endl;

  cockpitNodeResponseNak(getCockpitCommNode());
}

void CockpitNode::processMessageResponse(char *pMessage)
{
  if (fStringStartsWith(pMessage, "@IDENTIFY:"))
  {
    processNodeIdentify(pMessage);
  }
  else if (fStringStartsWith(pMessage, "@STATUS:"))
  {
    processNodeStatus(pMessage);
  }
  else if (fStringStartsWith(pMessage, "@ACK:"))
  {
    processNodeAck(pMessage);
  }
  else if (fStringStartsWith(pMessage, "@NAK:"))
  {
    processNodeNak(pMessage);
  }
  else
  {
    processNodeResponse(pMessage);
  }
}

// End of File
