/// Cockpit Node Controller firmware
///
/// Message handler interface

typedef void (*CommandProc_t)(const char *message);
typedef struct {
  const char *commandName;
  CommandProc_t commandProcessor;
} CommandRecord_t;

void SerialMessage_Create(void);
void SerialMessage_Destroy(void);

void SerialMessage_ReceiveEnable(void);
void SerialMessage_Process(CommandRecord_t *CommandTable);
flag SerialMessage_IsValid(void);

void SerialMessage_Transmit(const char *message);

/// End of File
