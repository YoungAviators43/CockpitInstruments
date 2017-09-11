/// File: CockpitPanel.hxx

#ifndef COCKPIT_PANEL_HXX
#define COCKPIT_PANEL_HXX

//static const int maxIndicatorTable = 64;
static const int maxIndicatorTable = 24;

typedef struct CockpitNodeComm CockpitNodeComm_t;

const char *commChannelGetPortPath(CockpitNodeComm_t *pCommChannel);

void ioControlAddNodeRead(CockpitNodeComm_t *pCommChannel, int fd);
void ioControlAddNodeWrite(CockpitNodeComm_t *pCommChannel, int fd);
void ioControlRemoveNodeWrite(CockpitNodeComm_t *pCommChannel, int fd);
void ioControlRemoveNode(CockpitNodeComm_t *pChannel, int fd);

void indicateNodeModified(CockpitNodeComm_t *pCommChannel);
void processCockpitNodeWrite(CockpitNodeComm_t *pCommChannel);
void processCockpitNodeRead(CockpitNodeComm_t *pCommChannel);
void processCockpitNodeHup(CockpitNodeComm_t *pCommChannel);

void initCockpitPanel(void);
void processCockpitIndicators(void);

char fStringStartsWith(const char *pS1, const char *pS2);
char isDecimalNumber(char ch);
void cockpitNodeResponseAck(CockpitNodeComm_t *pCommChannel);
void cockpitNodeResponseNak(CockpitNodeComm_t *pCommChannel);
void cockpitNodeResponseErr(CockpitNodeComm_t *pCommChannel);

#endif

/// End of File
