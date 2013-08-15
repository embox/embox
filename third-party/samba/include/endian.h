/*
 * endian.h
 *
 *  Created on: 15 août 2013
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

#endif /* ENDIAN_H_ */
