/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.05.2014
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text
	.extern vfork_body
	.global vfork

vfork:
	CALL_WPTREGS vfork_body
