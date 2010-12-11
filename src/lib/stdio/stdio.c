#include <stdio.h>
#include <drivers/vconsole.h>

FILE stdin = 0;
FILE stdout= 0;

static vconsole_t *cur_console;

