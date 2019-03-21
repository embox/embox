/**
 * @file
 *
 * @date Aug 2, 2018
 * @author Anton Bondarev
 */

#include <asm/linkage.h>

#define SYSCALL_NUMBER 1
/*
 * syscall.
 */
.text
.align 8
C_ENTRY(syscall):
{
	setwd wsz = 0x8
	setbn rsz = 0x3, rbs = 0x4, rcur = 0x0
	sdisp %ctpr1, SYSCALL_NUMBER
}
{
	addd %dr0, 0, %db[0]
	addd %dr1, 0, %db[1]
	addd %dr2, 0, %db[2]
	addd %dr3, 0, %db[3]
	addd %dr4, 0, %db[4]
}
{
	addd %dr5, 0, %db[5]
	addd %dr6, 0, %db[6]
	addd %dr7, 0, %db[7]
}
{
	call %ctpr1, wbs=0x4
}
{
	addd %db[0], 0, %dr0
	return %ctpr3
}
{
	ct %ctpr3
}
