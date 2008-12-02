#include "types.h"

// character to upper
char ch_upper(char ch)
{
  if (ch >= 'a' && ch <= 'z')
    return (ch + 'A' - 'a');
  else
    return ch;
}

// convert HEX digit character to integer
int ch_to_digit(char ch)
{
  ch = ch_upper(ch);
  if (ch >= '0' && ch <= '9')
  {
    return (ch - '0');
  } else if (ch >= 'A' && ch <= 'F') {
    return (ch - 'A' + 0x0A);
  }
  return 0;
}

int sz_length(const char * str)
{
  int i;
  for (i=0; str[i] != 0; i++)
    ;
  return i;
}

int sz_cpy(char * dest, const char * src)
{
  int count = 0;

  if (*src == 0)
    return 0;
  do {
    dest[count] = src[count];
  } while (src[count++] != '\0');
  return (count-1);
}

int sz_append(char *dest, const char *src, char ch)
{
	int i;
	for (i = 0; src[i] != 0; i++)
		dest[i] = src[i];

	if (i > 0)
		dest[i++] = '.';

	dest[i++] = ch;
	dest[i]   = 0x0;
	return i;
}

int sz_cmp(const char * str1, const char * str2)
{
  for (; *str1 == *str2; str1++, str2++)
  {
    if (*str1 == 0)
      return TRUE;
  }
  return FALSE;
}

WORD atow (char *buff)
{
  WORD result = 0;
  WORD paw = 1;
  int length = sz_length(buff) - 1;
  for(; length >= 0; length --)
  {
    result += ch_to_digit(buff[length]) * paw;
    paw = paw * 0x10;
  }
  return result;
}
