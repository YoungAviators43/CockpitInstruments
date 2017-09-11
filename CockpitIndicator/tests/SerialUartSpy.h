/// SerialUart spy

#include "SerialUart.h"

flag SerialUartSpy_IsUartCreated(void);
uint32_t SerialUartSpy_GetBaudRate(void);
char SerialUartSpy_GetLastTxChar(void);
void SerialUartSpy_RunTxbInterrupt(void);
void SerialUartSpy_RunTxcInterrupt(void);
void SerialUartSpy_RunRxbInterrupt(char ch);
void SerialUartSpy_RunRxbInterruptFrameError(char ch);
void SerialUartSpy_RunRxbInterruptOverrunError(char ch);

flag SerialUartSpy_IsTxbInterruptEnabled(void);
flag SerialUartSpy_IsTxcInterruptEnabled(void);
flag SerialUartSpy_IsTxLineEnable(void);
flag SerialUartSpy_IsRxbInterruptEnabled(void);

/// End of File
