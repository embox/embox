#ifndef _CONIO_H_
#define _CONIO_H_

#include "types.h"

// printf to standart output
int printf (const char *str, ...);

int sprintf(char *out, const char *format, ...);

// scanf from standart output
// keys: %[value]s - string (value == length)
//		 %c		   - char
//		 %[value]d - decimal (value == length)
//		 %[value]o - oct
//		 %[value]x - hex
// returns amount of converted entries
int scanf (const char *str, ...);

// sscanf from string
// keys: see above
// returns amount of converted entries
int sscanf (char *out, const char *format, ...);
//
void back_cursor (int count);
// back space
void back_space (int count);

void clr_scr ();

void cursor_home_pos ();

void cursor_save_pos ();

void cursor_restore_pos ();


#endif //_CONIO_H_
