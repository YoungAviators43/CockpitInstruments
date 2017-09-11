/// File: ProcessInstruments.cxx

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
using namespace std;

#include "Property.hxx"
#include "ProcessIndicators.hxx"

extern unsigned int logLevel;

static PropertyProcessNode_t indicatorNode[] =
  {
    { "/orientation/roll-deg",                                          "I01=%f", propFloat, 0.0, NULL },
    { "/orientation/pitch-deg",                                         "I02=%f", propFloat, 0.0, NULL },
    { "/instrumentation/slip-skid-ball/indicated-slip-skid",            "I03=%f", propFloat, 0.0, NULL },
    { "/instrumentation/turn-indicator/indicated-turn-rate",            "I04=%f", propFloat, 0.0, NULL },
    { "/instrumentation/altimeter/indicated-altitude-ft",               "I05=%f", propFloat, 0.0, NULL },
    { "/instrumentation/airspeed-indicator/indicated-speed-mph",        "I06=%f", propFloat, 0.0, NULL },
    { "/instrumentation/vertical-speed-indicator/indicated-speed-fpm",  "I07=%f", propFloat, 0.0, NULL },
    { "/instrumentation/magnetic-compass/indicated-heading-deg",        "I08=%f", propFloat, 0.0, NULL },
    { "/instrumentation/adf/indicated-bearing-deg",                     "I09=%f", propFloat, 0.0, NULL },
    { "/instrumentation/magnetic-compass/bug-heading-deg",              "I10=%f", propFloat, 0.0, NULL },
    { "/orientation/heading-magnetic-deg",                              "I11=%f", propFloat, 0.0, NULL },
    { "/engines/engine[0]/mp-inhg",                                     "I12=%f", propFloat, 25.0, NULL },
    { "/engines/engine[1]/mp-inhg",                                     "I13=%f", propFloat, 25.0, NULL },
    { "/engines/engine[0]/rpm",                                         "I14=%f", propFloat, 0.0, NULL },
    { "/engines/engine[1]/rpm",                                         "I15=%f", propFloat, 0.0, NULL },
    { "/engines/engine[0]/oil-pressure-psi",                            "I16=%f", propFloat, 0.0, NULL },
    { "/engines/engine[1]/oil-pressure-psi",                            "I17=%f", propFloat, 0.0, NULL },
    { "/engines/engine[0]/fuel-psi-norm",                               "I18=%f", propFloat, 0.0, NULL },
    { "/engines/engine[1]/fuel-psi-norm",                               "I19=%f", propFloat, 0.0, NULL },
    { "/engines/engine[0]/oil-temperature-degc",                        "I20=%f", propFloat, 20.0, NULL },
    { "/engines/engine[1]/oil-temperature-degc",                        "I21=%f", propFloat, 20.0, NULL },
    { "/engines/engine[0]/carb-temp-degc",                              "I22=%f", propFloat, 20.0, NULL },
    { "/engines/engine[1]/carb-temp-degc",                              "I23=%f", propFloat, 20.0, NULL },
    { "/engines/engine[0]/cyl-temp",                                    "I24=%f", propFloat, 20.0, NULL },
    { "/engines/engine[1]/cyl-temp",                                    "I25=%f", propFloat, 20.0, NULL },
    { "/environment/temperature-degc",                                  "I26=%f", propFloat, 20.0, NULL },
    { "/consumables/fuel/tank[0]/level-gal_us",                         "I27=%f", propFloat, 120.0, NULL },
    { "/consumables/fuel/tank[1]/level-gal_us",                         "I28=%f", propFloat, 128.0, NULL },
    { "/consumables/fuel/tank[2]/level-gal_us",                         "I29=%f", propFloat, 86.0, NULL },
    { "/consumables/fuel/tank[3]/level-gal_us",                         "I30=%f", propFloat, 92.0, NULL },
    { "/consumables/fuel/tank[4]/level-gal_us",                         "I31=%f", propFloat, 45.0, NULL },
    { "/consumables/fuel/tank[5]/level-gal_us",                         "I32=%f", propFloat, 54.0, NULL },
    { "/systems/vacuum/suction-inhg",                                   "I33=%f", propFloat, 0.0, NULL },
    { "/systems/hydraulics/pump-psi",                                   "I34=%f", propFloat, 0.0, NULL },
    { "/systems/hydraulic/brakes-psi",                                  "I35=%f", propFloat, 0.0, NULL },
    { "/surface-positions/flap-pos-norm",                               "I36=%f", propFloat, 0.0, NULL },
    { "/gear/gear[0]/position-norm",                                    "I37=%f", propFloat, 0.0, NULL },
    { "/gear/gear[1]/position-norm",                                    "I38=%f", propFloat, 0.0, NULL },
    { "/gear/gear[2]/position-norm",                                    "I39=%f", propFloat, 0.0, NULL },
    { NULL,                                                             NULL,     propFloat, 0.0, NULL }
  };


char getIndicatorPropertyStatus(int i, char *str)
{
  PropertyProcessNode_t *node;

  node = &indicatorNode[i];
  if (node->name != NULL)
  {
    sprintf(str, "%-64s %8.3f", node->name, node->propNode->getPropertyValueFloat());
  }
  return node->name != NULL;
}

void initIndicatorProperties(void)
{
  int i;

  for (i = 0; indicatorNode[i].name != NULL; i++)
  {
    indicatorNode[i].propNode = createPropertyNode(indicatorNode[i].name, indicatorNode[i].varType);
    indicatorNode[i].propNode->setPropertyValue(indicatorNode[i].defaultValue);
  }

  if (logLevel > 1)
    cout << "IndicatorProperties: processed " << i << " items." << endl;
}


static void processInputVar(int index, char *token)
{
  float x;

  if (sscanf(token, indicatorNode[index].scanFormat, &x) != 1)
  {
    if (logLevel > 1)
      cout << "input var error on " << indicatorNode[index].name << endl;
  }
  else
  {
    //    cout << "Update " << indicatorNode[index].name << " with " << x;
    assert(indicatorNode[index].propNode != NULL);
    if (logLevel > 1)
      cout << "[" << indicatorNode[index].propNode->getPropertyName() << "]";

    indicatorNode[index].propNode->setPropertyValue(x);

    if (logLevel > 1)
      cout << endl;
  }
}

static size_t str_split(char tokenList[][32], char *a_str)
{
  static char delim[3] = { ',', '\n', 0 };

  size_t idx  = 0;
  char* token = strtok(a_str, delim);

  while (token)
  {
    strcpy(tokenList[idx++], token);
    token = strtok(0, delim);
  }

  return idx;
}

static char tokens[50][32];

void processIndicators(char *pMessage)
{
  size_t count;
  size_t i;

  if (logLevel > 1)
    cout << "InputMessage{" << strlen(pMessage) << "}= " << pMessage << endl;

  count = str_split(tokens, pMessage);

  if (logLevel > 1)
    cout << "Items found: " << count << endl;
  for (i = 0; i < count; i++)
  {
    if (logLevel > 1)
      cout << "item=[" << tokens[i] << "]{" << strlen(tokens[i]) << "}:  ";

    processInputVar(i, tokens[i]);
  }
}
