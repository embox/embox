/*
 * test_soft_traps.c
 *
 *  Created on: 26.06.2009
 *      Author: sunnya
 */
volatile static int temp;

int test_soft_traps()
{
    temp = -1;
    __asm__ ("mov temp, %o0");
    __asm__ ("ta 0x10");
    return temp;
}
