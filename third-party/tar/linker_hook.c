#include <time.h>

void (*gmtime_r_linker_hook)() = (void (*)())gmtime_r;
void (*localtime_r_linker_hook)() = (void (*)())localtime_r;
