// default string length
#define SZMAX   64

// character to upper
char ch_upper(char ch);

// character to integer conversion
int ch_to_digit(char ch);

int sz_length(const char *str);

int sz_cpy(char *dest, const char *src);

int sz_append(char *dest, const char * src, char ch);

int sz_cmp(const char *str1, const char *str2);

