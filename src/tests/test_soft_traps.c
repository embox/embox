/*
 * test_soft_traps.c
 *
 *  Created on: 26.06.2009
 *      Author: sunnya
 */
volatile static int temp;

int test_soft_traps()
{
    temp = -1;  //%l7
    __asm__ ("ta 0x10");
    __asm__ ("mov %l7, %o0");

    __asm__ ("mov %o0, %l7");
    return temp;
}
