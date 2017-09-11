/// Cockpit Node Controller firmware
///
/// Message parsing functions.

#include "CommonDefs.h"

const char *parseIntegerValue(const char *str, int16_t *pValue);
flag fStringStartsWith(const char *pS1, const char *pS2);
flag fIsDecimalNumber(char ch);

flag fMessageCodeValid(char *message);

/// End of File
