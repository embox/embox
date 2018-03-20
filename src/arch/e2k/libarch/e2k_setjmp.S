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
	rrd %cr0.hi, [%dr0]
	rrd %cr1.lo, [%dr0]
	rrd %cr1.hi, [%dr0]
	rrd %usd.lo, [%dr0]
	rrd %usd.hi, [%dr0]

.size $setjmp, . - $setjmp


.global $longjmp
.type longjmp,@function
$longjmp:
	nop 5
	rwd [%dr0], %cr0.hi
	rwd [%dr0], %cr1.lo
	rwd [%dr0], %cr1.hi
	rwd [%dr0], %usd.lo
	rwd [%dr0], %usd.hi

.size $longjmp, . - $longjmp
