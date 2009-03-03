// default string length
#define SZMAX   64

// character to upper
char ch_upcase(char ch);

// character to integer conversion
int ch_to_digit(char ch, int base);

int strlen(const char *str);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *source, size_t count);

//int sz_append(char *dest, const char * src, char ch);

int strcmp(const char *str1, const char *str2);

int strncmp(const char *s1, const char *s2, size_t count);

// Determines whether beg is the beginning of the str string
int str_starts_with(const char *str, const char *beg, int len);

