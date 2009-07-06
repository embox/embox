/**
 * \file test_soft_traps.c
 *
 * \date 26.06.2009
 * \author sunnya
 */
#include "common.h"
#include "express_tests.h"

unsigned int volatile test_soft_traps_variable;
#define TEST_SOFT_TRAP_NUMBER 0x10 //trap 0x90

#define TEST_NAME "soft traps"
static char *get_test_name(){
    return TEST_NAME;
}

static int exec() {
    unsigned int temp = test_soft_traps_variable;
    __asm__ __volatile__  ("ta %0\n\t"::"i" (TEST_SOFT_TRAP_NUMBER));

    if (temp != (test_soft_traps_variable - 1)){
        TRACE("Incorrect software traps handling\n");
        return -1;
    }
    return 0;
}

static EXPRESS_TEST_HANDLER exp_handler = {get_test_name, exec};
REGISTER_EXPRESS_TEST(&exp_handler);
