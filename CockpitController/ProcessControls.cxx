/// File: ProcessControls.cxx

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
//#include "ProcessIndicators.hxx"

extern unsigned int logLevel;

static PropertyProcessNode_t controllerNode[] =
  {
//    { "/controls/flight/aileron",                              "C01=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/flight/aileron-trim",                         "C02=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/flight/elevator",                             "C03=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/flight/elevator-trim",                        "C04=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/flight/rudder",                               "C05=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/flight/rudder-trim",                          "C06=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/flight/flaps",                                "C07=%5.3f,", propFloat, 0.0, NULL },
    { "/controls/engines/engine[0]/throttle",                  /*"C08=%5.3f,"*/"%5.3f,", propFloat, 0.15, NULL },
//    { "/controls/engines/engine[0]/starter",                   "C09=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/fuel-pump",                 "C10=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/primer",                    "C11=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/fire-switch",               "C12=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/fire-bottle-discharge",     "C13=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/cutoff",                    "C14=%5.3f,", propFloat, 0.0, NULL },
    { "/controls/engines/engine[0]/mixture",                   /*"C15=%5.3f,"*/"%5.3f,", propFloat, 0.9, NULL },
    { "/controls/engines/engine[0]/propeller-pitch",           /*"C16=%5.3f,"*/"%5.3f,", propFloat, 0.9, NULL },
//    { "/controls/engines/engine[0]/magnetos",                  "C17=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/boost",                     "C18=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/cowl-flaps-cmd",            "C19=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/feather",                   "C20=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/ignition",                  "C21=%5.3f,", propFloat, 0.0, NULL },
    { "/controls/engines/engine[1]/throttle",                  /*"C22=%5.3f,"*/"%5.3f,", propFloat, 0.15, NULL },
//    { "/controls/engines/engine[1]/starter",                   "C23=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/fuel-pump",                 "C24=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/primer",                    "C25=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/fire-switch",               "C26=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/fire-bottle-discharge",     "C27=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/cutoff",                    "C28=%5.3f,", propFloat, 0.0, NULL },
    { "/controls/engines/engine[1]/mixture",                   /*"C29=%5.3f,"*/"%5.3f,", propFloat, 0.9, NULL },
    { "/controls/engines/engine[1]/propeller-pitch",           /*"C30=%5.3f,"*/"%5.3f,", propFloat, 0.9, NULL },
//    { "/controls/engines/engine[1]/magnetos",                  "C31=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/boost",                     "C32=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/cowl-flaps-cmd",            "C33=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/feather",                   "C34=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/ignition",                  "C35=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/fuel/switch-position",                        "C36=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/fuel/left-valve",                             "C37=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/fuel/right-valve",                            "C38=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/gear/gear-down",                              "C39=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/gear/brake-left",                             "C40=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/gear/brake-right",                            "C41=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/gear/brake-parking",                          "C42=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/anti-ice/wing-heat",                          "C43=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/anti-ice/pitot-heat",                         "C44=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/anti-ice/prop-heat",                          "C45=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/anti-ice/engine[0]/carb-heat",                "C46=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/anti-ice/engine[1]/carb-heat",                "C47=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/cabin/fan",                                   "C48=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/cabin/heat",                                  "C49=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/instrument-lights",                  "C50=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/electric/battery-switch",                     "C51=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/electric/external-power",                     "C52=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/electric/engine[0]/generator",                "C53=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/master-alt",                "C54=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[0]/master-bat",                "C55=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/electric/engine[1]/generator",                "C56=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/master-alt",                "C57=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/engines/engine[1]/master-bat",                "C58=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/landing-lights[0]",                  "C59=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/landing-lights[1]",                  "C60=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/passing-lights",                     "C61=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/tail-lights",                        "C62=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/beacon",                             "C63=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/strobe",                             "C64=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/panel-norm",                         "C65=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/instruments-norm",                   "C66=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/running-lights",                     "C67=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/formation-lights",                   "C68=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/recognition-lights[0]",              "C69=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/recognition-lights[1]",              "C70=%5.3f,", propFloat, 0.0, NULL },
//    { "/controls/lighting/recognition-lights[2]",              "C71=%5.3f,", propFloat, 0.0, NULL },
    { NULL,                                                    NULL,     propFloat, 0.0, NULL }
  };


char getControlPropertyStatus(int i, char *str)
{
  PropertyProcessNode_t *node;

  node = &controllerNode[i];
  if (node->name != NULL)
  {
    sprintf(str, "%-64s %8.3f", node->name, node->propNode->getPropertyValueFloat());
  }
  return node->name != NULL;
}


void initControlProperties(void)
{
  int i;

  for (i = 0; controllerNode[i].name != NULL; i++)
  {
    controllerNode[i].propNode = createPropertyNode(controllerNode[i].name, controllerNode[i].varType);
    controllerNode[i].propNode->setPropertyValue(controllerNode[i].defaultValue);
  }

  if (logLevel > 1)
    cout << "ControllerProperties: processed " << i << " items." << endl;
}

//void displayFgWriteMsg(int nItems, char *msg);

void processControls(char *pMessage)
{
  int nBytes;
  int totalBytes;
  size_t i;

  totalBytes = 0;
  for (i = 0; controllerNode[i].name != NULL; i++)
  {
    nBytes = sprintf(&pMessage[totalBytes],
		     controllerNode[i].scanFormat,
		     controllerNode[i].propNode->getPropertyValueFloat());
    assert(nBytes >= 0);
    //    pMessage += nBytes;
    totalBytes += nBytes;
    //    *pMessage++ = ',';
  }

  assert(pMessage[totalBytes - 1] == ',');
  pMessage[totalBytes - 1] = '\n';  // replace last ',' with '\n'

#if 0
  displayFgWriteMsg(totalBytes, pMessage);
#endif
}
