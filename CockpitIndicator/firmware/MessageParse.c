/// Cockpit Node Controller firmware
///
/// Message parsing routines.

#include "CommonDefs.h"
#include "MessageParse.h"

const char *parseIntegerValue(const char *str, int16_t *pValue)
{
  flag fNegative;
  int16_t value;

  value = 0;
  fNegative = (*str == '-');
  if ((*str == '+') || (*str == '-'))
    str += 1;
  while (fIsDecimalNumber(*str))
  {
    value = value * 10 + (*str - '0');
    str += 1;
  }
  if (*str == '.')
  {
    str += 1;
    while (fIsDecimalNumber(*str))
      str += 1;
  }
  if (*str == ',')
    str += 1;

  *pValue = fNegative ? (- value) : (value) ;
  return str;
}

flag fStringStartsWith(const char *pS1, const char *pS2)
{
  while (*pS2 != '\0' && *pS2 == *pS1)
  {
    pS1 += 1;
    pS2 += 1;
  }
  return *pS2 == '\0';
}

flag fIsDecimalNumber(char ch)
{
  switch (ch)
  {
    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
      return TRUE;
    default:
      return FALSE;
  }
}

flag fMessageCodeValid(char *message)
{
  int charCount;
  int checkCount;
  char *pColon;
  flag fOk;

  fOk = FALSE;

  // Count characters in the message.
  charCount = 0;
  pColon = message;
  while (*pColon != '\0')
  {
    charCount += 1;
    pColon += 1;
  }

  if (charCount > 0)
  {
    // Step backwards until ':' is found.
    do {
      pColon -= 1;
      charCount -= 1;
    } while ((charCount > 0) && (*pColon != ':'));

    if (*pColon == ':')
    {
      // Move past the ':'
      pColon += 1;

      // Parse the checkcount.
      checkCount = 0;
      while (fIsDecimalNumber(*pColon))
      {
	checkCount = checkCount * 10 + (*pColon - '0');
	pColon += 1;
      }
      if (*pColon == '\0')
	fOk = (charCount == checkCount);
    }
  }
  return fOk;
}

