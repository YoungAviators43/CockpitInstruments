/// File: Property.cxx
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"

extern unsigned int logLevel;

const char *getPropertyTypeName(PropertyType_t pType)
{
  switch (pType)
  {
    case propFloat:
      return "<Float>";
    case propDouble:
      return "<Double>";
    case propInt:
      return "<Integer>";
    case propBool:
      return "<Boolean>";
    default:
      return "<PropertyType: unkown>";
  }
}

class SubscriberNode
{
private:
  CockpitNode *pNode;
  SubscriberNode *pNext;

public:
  SubscriberNode(CockpitNode *pCockpitNode, SubscriberNode *pNextSubscriber) :
    pNode(pCockpitNode), pNext(pNextSubscriber) { };

  SubscriberNode *notifySubscriber(void) { notifyCockpitNode(pNode);  return pNext; };

  char isNodeSubscriber(CockpitNode *pSubscriberNode) { return pSubscriberNode == pNode; };
  SubscriberNode *getNext(void) { return pNext; };
  void setNext(SubscriberNode *pNewNext) { pNext = pNewNext; };
};

PropertyNode::PropertyNode(const char *propertyName, PropertyType_t propertyType) :
  pType(propertyType), pValue(0.0), pRootSubscriber(NULL)
{
  assert(strlen(propertyName) < maxPropertyName);

  strcpy(this->pName, propertyName);
}

void PropertyNode::addSubscriber(CockpitNode *pNode)
{
  //assert(findSubscriber(pNode) == NULL);   // check not already subscribed
  pRootSubscriber = new SubscriberNode(pNode, pRootSubscriber);
  assert(pRootSubscriber != NULL);

  if (logLevel > 1)
    cout << "Info: " << cockpitNodeName(pNode) << " subscribed to " << pName << endl;
}

void PropertyNode::removeSubscriber(CockpitNode *pNode)
{
  SubscriberNode *pSubscriberNode;
  SubscriberNode *pNext;

  pSubscriberNode = pRootSubscriber;
  if (pSubscriberNode != NULL)
  {
    if (pSubscriberNode->isNodeSubscriber(pNode))
    {
      pRootSubscriber = pSubscriberNode->getNext();
      delete(pSubscriberNode);
    }
    else
    {
      pNext = pSubscriberNode->getNext();
      while (pNext != NULL && !pNext->isNodeSubscriber(pNode))
      {
	pSubscriberNode = pNext;
	pNext = pNext->getNext();
      }
      if (pNext != NULL)
      {
	pSubscriberNode->setNext(pNext->getNext());
	delete(pNext);
      }
    }
  }
}

void PropertyNode::notifySubscribers(void)
{
  SubscriberNode *pNode;

  pNode = pRootSubscriber;
  while (pNode != NULL)
  {
    pNode = pNode->notifySubscriber();
  }
}

void PropertyNode::setPropertyValue(float newValue)
{
  if (pValue != newValue)
  {
    if (logLevel > 1)
      cout << "setPropertyValue(" << pName << "): " << pValue << " => " << newValue << " (notify subscribers)" << endl;

    pValue = newValue;
    notifySubscribers();
  }
}

const int maxPropertyList = 128;
static int sizePropertyList = 0;
static PropertyNode *propertyList[maxPropertyList];

static void propertyAddNode(PropertyNode *pNode)
{
  assert(pNode != NULL);
  assert(sizePropertyList < maxPropertyList);

  propertyList[sizePropertyList] = pNode;
  sizePropertyList += 1;
}

static PropertyNode *findPropertyNode(const char *propertyName)
{
  int i;
  PropertyNode *pResultNode;

  pResultNode = 0; // null
  for (i = 0; i < sizePropertyList; i++)
  {
    if (strcmp(propertyName, propertyList[i]->getPropertyName()) == 0)
    {
      pResultNode = propertyList[i];
    }
  }

  return pResultNode;
}

PropertyNode *createPropertyNode(const char *propertyName, PropertyType_t propertyType)
{
  PropertyNode *pNode;

  pNode = findPropertyNode(propertyName);
  if (pNode != NULL)
  {
    // Found existing node.  Check that the format matches.
    assert(pNode->getPropertyType() == propertyType);

    if (logLevel > 2)
      cout << "getPropertyNode(" << propertyName << "): found." << endl;
  }
  else
  {
    if (logLevel > 2)
      cout << "getPropertyNode(" << propertyName << "): not found, create." << endl;

    pNode = new PropertyNode(propertyName, propertyType);
    assert(pNode != NULL);
    propertyAddNode(pNode);
  }

  return pNode;
}

PropertyNode *getPropertyNode(const char *propertyName)
{
  return findPropertyNode(propertyName);
}
 

/// End of File: Property.cxx
