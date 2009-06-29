/*
 * test_sys_exec.c
 *
 *  Created on: 29.06.2009
 *      Author: sunnya
 */

#include "test_sys_exec.h"

static int func(int argc, char **argv) {
    printf("in test sys exec\n");
    return 0;
}

int test_sys_exec(int argc, char **argv){
    if (!sys_exec(func, argc, argv))
        return 0;
    return -1;
}
