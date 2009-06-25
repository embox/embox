#include "leon_config.h"
#include "memory_map.h"


	.section    ".text"
	.align  4

	.global boot_start, warm_start
boot_start:
	! SPARC v8 initialization
	flush                           ! flush caches

	set 0xC0, %g1                 ! disable traps
	mov %g1, %psr
	nop;nop;nop

	mov %g0, %wim
	mov %g0, %tbr
	mov %g0, %y

	nop;nop;nop

	flush                           ! flush caches
	nop
	nop
	nop
	set 0x81000F, %g1
	sta %g1, [%g0] 2
!clear global register
	clr %g1
	clr %g2
	clr %g3
	clr %g4
	clr %g5
	clr %g6
	clr %g7


	! clean all register windows
	set CORE_NWINDOWS, %g1
register_window_clean:
	mov %g0, %l0
	mov %g0, %l1
	mov %g0, %l2
	mov %g0, %l3
	mov %g0, %l4
	mov %g0, %l5
	mov %g0, %l6
	mov %g0, %l7
	mov %g0, %o0
	mov %g0, %o1
	mov %g0, %o2
	mov %g0, %o3
	mov %g0, %o4
	mov %g0, %o5
	mov %g0, %o6
	mov %g0, %o7
	deccc %g1
 	bg register_window_clean
	save       !
	nop


	set		__stack, %g1
	mov     %g1, %fp                  ! set frame pointer
	sub     %g1, 0x60, %sp            ! set stack pointer

	//set     STACK_BASE-0x10, %g1
	//mov     %g1, %fp                  ! set frame pointer
	//sub     %g1, 0x60, %sp            ! set stack pointer


	set     _trap_table, %g1
	mov     %g1, %tbr               !set trap table addr
	nop; nop; nop;


	mov	2, %g1
	mov	%g1, %wim   !
	set 0xe0, %g1	!enable traps
	mov %g1, %psr   !
	nop;
	nop;
	nop;

	call	main
	nop


    nop
    ta 0
    nop

