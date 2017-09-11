/// Cockpit Node Controller firmware
///
/// Uart controller interface.

#include "CommonDefs.h"

void SerialUart_Create(void);
void SerialUart_Initialize(void);

void SerialUart_EnableTxb(void (*TxbCallbackFunction)(void));
void SerialUart_DisableTxb(void);
void SerialUart_TxChar(char ch);

void SerialUart_EnableRxb(void (*RxbCallbackFunction)(void));
void SerialUart_DisableRxb(void);
flag SerialUart_fRxGetChar(char *pCh);

/// End of File
