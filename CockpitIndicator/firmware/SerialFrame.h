/// Cockpit Node Controller firmware
/// SerialFrame interface.

#include "CommonDefs.h"

void SerialFrame_Create(void);

void SerialFrame_Destroy(void);

flag SerialFrame_IsRxIdle(void);

flag SerialFrame_IsTxIdle(void);

void SerialFrame_Initialize(void);

flag SerialFrame_TransmitReady(void);

void SerialFrame_Transmit(const char *message);

void SerialFrame_Receive(char *buffer, uint8_t bufferSize);

flag SerialFrame_IsRxMessageComplete(void);

/// End of File
