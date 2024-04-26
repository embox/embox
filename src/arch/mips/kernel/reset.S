/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 13.09.2012
 */

#include <asm/asm.h>

.section .reset, "x"
.align 4
.global _reset
_reset:
	PTR_LA  $k0, _start
	jr      $k0 # Jump to startup code
	nop
