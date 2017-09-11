/// File: Property.hxx
///
///

#ifndef PROPERTY_H
#define PROPERTY_H

#include "CockpitNode.hxx"

typedef enum { propFloat, propDouble, propInt, propBool } PropertyType_t;

class SubscriberNode;

const int maxPropertyName = 64;

class PropertyNode
{
  char pName[maxPropertyName];
  PropertyType_t pType;
  float pValue;   // make this a morphable type
  SubscriberNode *pRootSubscriber;

public:
  PropertyNode(const char *propertyName, PropertyType_t propertyType);

  char *getPropertyName(void) { return pName; };
  PropertyType_t getPropertyType(void) { return pType; };
  float getPropertyValueFloat(void) { return pValue; };
  void addSubscriber(CockpitNode *pNode);
  void removeSubscriber(CockpitNode *pNode);
  void notifySubscribers(void);

  void setPropertyValue(float newValue);

};

PropertyNode *createPropertyNode(const char *propertyName, PropertyType_t propertyType);
PropertyNode *getPropertyNode(const char *propertyName);

typedef struct {
  const char *name;
  const char *scanFormat;
  PropertyType_t varType;
  float defaultValue;
  PropertyNode *propNode;
} PropertyProcessNode_t;


const char *getPropertyTypeName(PropertyType_t pType);

#endif
