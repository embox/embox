/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */
#include <asm/setjmp.h>

.section ".text", "ax"
.global $setjmp
.type setjmp,@function
$setjmp:
.size $setjmp, . - $setjmp

.global $longjmp
.type longjmp,@function
$longjmp:
.size $longjmp, . - $longjmp
