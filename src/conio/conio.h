#ifndef _CONIO_H_
#define _CONIO_H_


// printf to standart output
int printf (const char *str, ...);

int sprintf(char *out, const char *format, ...);
// scanf from standart output
int scanf (const char *str, ...);
//
void back_cursor (int count);
// back space
void back_space (int count);

void clr_scr ();

void cursor_home_pos ();

void cursor_save_pos ();

void cursor_restore_pos ();

void show_mem_prompt ();

// run if wrong character
void wrong_char(char ch);

// ask accept (input: 'Y' - yes, 'N' - no)
BOOL ask_accept();

//show mean in addr with iow access
BOOL show_mem_mean (UINT32 addr, int iow);

//show mean in addr with iow access write value
//adn show mean agein
BOOL show_mem_write (UINT32 addr, int iow);

//show message end value mean than put hex string
//return TRUE if value is valid
BOOL get_hex_value(char *message, UINT32 *value);

BOOL get_hex_str (char *str);


void gets(char *buff, int size);


#endif //_CONIO_H_
