/// File: CockpitNode.hxx
///
/// This is the public implementation file for the CockpitNode class.
/// See the file: CockpitNodeInternal.hxx for further implementation information.
///

#ifndef COCKPITNODE_H
#define COCKPITNODE_H

class CockpitNode;

void notifyCockpitNode(CockpitNode *pCockpitNode);
const char *cockpitNodeName(CockpitNode *pCockpitNode);
void processCockpitNodeWrite(CockpitNode *pNode);
void processCockpitNodeRead(CockpitNode *pNode);


#endif
