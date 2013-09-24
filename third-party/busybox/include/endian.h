/*
 * endian.h
 *
 *  Created on: 2013-sep-20
 *      Author: fsulima
 */

#ifndef ENDIAN_H_
#define ENDIAN_H_

static void __x86_verificator__(void) {
	// This is to make sure this header only compiles on x86
	asm ("mov %cr2, %eax");
}
#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412
// x86 architecture only
#define __BYTE_ORDER __LITTLE_ENDIAN

#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENGIAN    __BIG_ENDIAN
#define PDP_ENDIAN    __PDP_ENDIAN
#define BYTE_ORDER    __BYTE_ORDER

#endif /* ENDIAN_H_ */
