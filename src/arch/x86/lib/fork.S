/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.05.2014
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text
	.extern fork_body
	.global fork

fork:
	CALL_WPTREGS fork_body
