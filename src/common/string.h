// default string length
#define SZMAX   64

// character to upper
char ch_upcase(char ch);

// character to integer conversion
int ch_to_digit(char ch, int base);

int sz_length(const char *str);

int sz_cpy(char *dest, const char *src);

int sz_append(char *dest, const char * src, char ch);

int sz_cmp(const char *str1, const char *str2);

// Determines whether beg is the beginning of the str string
int sz_cmp_beginning(const char *beg, const char *str);

