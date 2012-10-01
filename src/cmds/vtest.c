/*
 * vtest.c
 *
 *  Created on: Sep 28, 2012
 *      Author: user
 */

#include <embox/cmd.h>
#include <stdio.h>
#include <vesa.h>
#include <asm/io.h>

EMBOX_CMD (main);

static inline unsigned get_seg (void * ptr) {
	return 0;
}

static inline unsigned get_off (void * ptr) {
	return 0;
}

static inline void * mk_farptr (unsigned seg, unsigned off) {
	return 0;
}

static inline void pokeb (unsigned seg, unsigned off, unsigned char val) {
}

static inline unsigned char peekb (unsigned seg, unsigned off) {
	return 0;
}

int main (int argc, char ** argv)
{
	unsigned c = 0xC;
	printf ("%d\n", c >> 2);
	return 0;
}
