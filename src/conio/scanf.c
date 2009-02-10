/*
 * scanf.c
 *	Realization of function scanf
 *  Created on: 10.02.2009
 *      Author: abatyukov
 */
#include "types.h"
#include "conio.h"
#include "stdarg.h"

int getchar ()
{
	static char prev = 0;

	prev = uart_getc();
	return (int) prev;
}

static void scanchar(char **str)
{
   extern int getchar();
   if (str) {
      **str = getchar();
      ++(*str);
   }
   else (void)getchar();
}


#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **in, const char *string, int width, int pad)
{
   int sc = 0, padchar = ' ';
   if (width > 0) {
      int len = 0;
      const char *ptr;
      for (ptr = string; *ptr; ++ptr) ++len;
      if (len >= width) width = 0;
      else width -= len;
      if (pad & PAD_ZERO) padchar = '0';
   }
   if (!(pad & PAD_RIGHT)) {
      for ( ; width > 0; --width) {
     scanchar (in, padchar);
     ++sc;
      }
   }
   for ( ; *string ; ++string) {
      scanchar (in, *string);
      ++sc;
   }
   for ( ; width > 0; --width) {
      scanchar (in, padchar);
      ++sc;
   }
   return sc;
}


/* the following should be enough for 32 bit int */

#define SCAN_BUF_LEN 12

static int scani(char **in, int i, int b, int sg, int width, int pad, int letbase)
{
   char scan_buf[SCAN_BUF_LEN];
   char *s;
   int t, neg = 0, sc = 0;
   unsigned int u = i;
   if (i == 0) {
      scan_buf[0] = '0';
      scan_buf[1] = '\0';
      return scans (out, print_buf, width, pad);
   }
   if (sg && b == 10 && i < 0) {
      neg = 1;
      u = -i;
   }
   s = scan_buf + SCAN_BUF_LEN-1;
   *s = '\0';
   while (u) {
      t = u % b;
      if( t >= 10 )
     t += letbase - '0' - 10;
      *--s = t + '0';
      u /= b;
   }
   if (neg) {
      if( width && (pad & PAD_ZERO) ) {
     scanchar (in,'-');
     ++sc;
     --width;
      }
      else {
     *--s = '-';
      }
   }
   return sc + scans (in, s, width, pad);
}


static int scan(char **in, int *varg)
{
   int width, pad;
   int sc = 0;
   char *format = (char *)(*varg++);
   char scr[2];

   for (; *format != 0; ++format) {
      if (*format == '%') {
     ++format;
     width = pad = 0;
     if (*format == '\0') break;
     if (*format == '%') goto in;
     if (*format == '-') {
        ++format;
        pad = PAD_RIGHT;
     }
     while (*format == '0') {
        ++format;
        pad |= PAD_ZERO;
     }
     for ( ; *format >= '0' && *format <= '9'; ++format) {
        width *= 10;
        width += *format - '0';
     }
     if( *format == 's' ) {
        char *s = *((char **)varg++);
        sc += scans (in, s?s:"(null)", width, pad);
        continue;
     }
     if( *format == 'd' ) {
        sc += scani (in, *varg++, 10, 1, width, pad, 'a');
        continue;
     }
     if( *format == 'x' ) {
        sc += scani (in, *varg++, 16, 0, width, pad, 'a');
        continue;
     }
     if( *format == 'X' ) {
        sc += scani (in, *varg++, 16, 0, width, pad, 'A');
        continue;
     }
     if( *format == 'u' ) {
        sc += scani (in, *varg++, 10, 0, width, pad, 'a');
        continue;
     }
     if( *format == 'c' ) {
/* char are converted to int then pushed on the stack */
        scr[0] = *varg++;
        scr[1] = '\0';
        sc += scans (in, scr, width, pad);
        continue;
     }
      }
      else {
    in:
     scanchar (in, *format);
     ++sc;
      }
   }
   if (in) **in = '\0';
   return sc;
}


/* assuming sizeof(void *) == sizeof(int) */
int scanf(const char *format, ...)
{
   int *varg = (int *)(&format);
   return scan(0, varg);
}


int sscanf(char *out, const char *format, ...)
{
   int *varg = (int *)(&format);
   return scan(&out, varg);
}

