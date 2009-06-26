/*
 * test_soft_traps.c
 *
 *  Created on: 26.06.2009
 *      Author: sunnya
 */
unsigned int volatile test_soft_traps_variable;
#define TEST_SOFT_TRAP_NUMBER 0x10 //trap 0x90
int test_soft_traps()
{
    unsigned int temp = test_soft_traps_variable;  //%l7
    __asm__ __volatile__  ("ta %0\n\t"::"i" (TEST_SOFT_TRAP_NUMBER));

    if (temp != (test_soft_traps_variable + 1)){
        printf ("gfhgk");
        return -1;
    }
    return 0;
}
