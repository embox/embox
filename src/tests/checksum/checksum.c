/**
 * \file checksum.c
 * \date 09.07.09
 * \author sikmir
 */

#include "common.h"
#include "string.h"

#if 0
static void sign_checksum(void) {
	__asm__(".section .checksum\n"
		".global _md5_sum\n"
		"_md5_sum: \n"
		".string \""
		MD5_CHECKSUM
		"\"\n"
		".section .text\n");
}
#endif
