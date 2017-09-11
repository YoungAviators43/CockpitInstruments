
#ifndef COCKPITNODEINTERNAL_H
#define COCKPITNODEINTERNAL_H

#include "Property.hxx"
#include "CockpitPanel.hxx"

class CockpitNode
{
private:
  const char *cockpitNodeName;
  CockpitNodeComm_t *cockpitCommNode;

protected:
  char infoStr[64];

  CockpitNodeComm_t *getCockpitCommNode(void) { return cockpitCommNode; };
  void processUpdate(void);
  virtual void composeSetIndicatorMsg(char *buffer) {  };
  void sendNodeMessage(const char *message);

  void indicatorPropertyNodeNotFound(const char *pPropertyName);
  char isPropertyTypeCorrect(PropertyNode *pPropertyNode, PropertyType_t pType);
  void propertyNodeUnregister(PropertyNode *pPropertyNode);
  virtual void processNodeResponse(char *message) { cockpitNodeResponseAck(getCockpitCommNode()); };
  void processNodeIdentify(char *msg);
  virtual const char *nodeTypeCode(void) { return "***"; };
  virtual const char *nodeFullName(void) { return "<unknown>"; };
  virtual void processNodeStatus(char *msg);
  virtual void processNodeAck(char *msg);
  virtual void processNodeNak(char *msg);
  virtual void fetchNodeMessage(char *buffer) { *buffer = '\0'; };

  PropertyNode *acquireCockpitNodeProperty(const char *propertyName, PropertyType_t pType);
  PropertyNode *registerCockpitNodeProperty(const char *propertyName, PropertyType_t pType);

public:
  CockpitNode(CockpitNodeComm_t *pCommNode, const char *nodeName);
  virtual ~CockpitNode() { };
  const char *getCockpitNodeName(void) { return cockpitNodeName; };
  const char *getCockpitNodeCode(void) { return nodeTypeCode(); };
  void NotifyModified(void);
  virtual void processNotification(void) { cout << getCockpitNodeName() << ": Null process notification." << endl; };
  virtual const char *getCockpitNodeInfo(void) { return ""; };
  void indicateUpdateRequired(void);
  void composeNodeMessage(char *buffer);
  void processMessageResponse(char *message);

};

#endif
