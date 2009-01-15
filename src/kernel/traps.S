#include "leon_config.h"

#include "traps.inc"

#define SA(x)		(((x)+7)&(~0x07))	/* rounds "x" up to doubleword boundary */
#define KERNEL_FRAME	(16*4)			/* size of frame containing only %i and %l registers */
#define USER_FRAME	((16+1+6)*4)		/* minimum size frame by regular software considerations */

! skip instruction
skip:
	flush
	jmpl    %l2, %g0
	rett    %l2 + 4
	nop
	nop

irq_empty:
	jmp		%l1	! old PC
	 rett	%l2	! old nPC
	nop
	nop

/* Window overflow trap handler. */
		.global window_overflow
window_overflow:
	mov	%g1, %l7		! Remember previous value of %g1 to restore it later

	mov	%wim, %g1		! Calculate new WIM
	srl	%g1, 1, %l3
	sll	%g1, CORE_NWINDOWS-1 , %g1
	or	%l3, %g1, %g1	! Circular right shifting by 1 bit

	save		! Get into window to be saved
	mov	%g1, %wim	! Write new WIM value that has been just calculated
	std	%l0, [%sp + 0x00];
	std	%l2, [%sp + 0x08];
	std	%l4, [%sp + 0x10];
	std	%l6, [%sp + 0x18];
	std	%i0, [%sp + 0x20];
	std	%i2, [%sp + 0x28];
	std	%i4, [%sp + 0x30];
	std	%i6, [%sp + 0x38];
	restore		! Go back to trap window

	mov	%l7, %g1	! Restore saved %g1
	jmp	%l1		! Re-execute save
	 rett	%l2

/* Window underflow trap handler.  */
		.global  window_underflow
window_underflow:

	mov	%wim, %l3		! Calculate new WIM
	sll	%l3, 1, %l4
	srl	%l3, CORE_NWINDOWS-1, %l3
	or	%l3, %l4, %l3	! Circular left shifting by 1 bit
	mov	%l3, %wim		! Write new WIM value
	nop; nop; nop		! Wait for WIM delayed writing

	restore		! Two restores to get into the
	restore		! window to be restored
	ldd	[%sp + 0x00], %l0;
	ldd	[%sp + 0x08], %l2;
	ldd	[%sp + 0x10], %l4;
	ldd	[%sp + 0x18], %l6;
	ldd	[%sp + 0x20], %i0;
	ldd	[%sp + 0x28], %i2;
	ldd	[%sp + 0x30], %i4;
	ldd	[%sp + 0x38], %i6;
	save
	save		! Get back to trap window

	jmp  %l1	! Re-execute restore
	 rett  %l2

  /* Flush windows routine.  */
        .global flush_windows
flush_windows:

   	mov	%psr, %g1
	or	%g1, 0x0f00, %g2
	restore				! enter previous frame (cannot trap)
        wr      %g2, 0x20, %psr		! enable traps, disable interrupts
	nop; nop; nop
	save				! 6 save to flush all windows
	save
	save
	save
	save
	save
	restore				! 5 restore to enter trapped frame
	restore
	restore
	restore
	restore
	mov      %g1, %psr		! restore %psr
	nop; nop; nop
  	jmp  %l2			! Jump to nPC
  	rett  %l2 + 4



spil:
        mov     %psr, %l4
        srl     %l4, 8, %l5
        andn    %l4, 0x0f00, %l4
        sll     %i0, 8, %i0
        and     %i0, 0x0f00, %i0
        or      %i0, %l4, %l4
        mov     %l4, %psr
        and     %l5, 0x0f, %i0
        jmpl    %l2, %g0
        rett    %l2 + 4

rtc_trap:
        !set     io_rtc_alarm, %l4
        !set     0x01, %l5
        !stb     %l5, [%l4]

        jmpl    %l1, %g0
         rett    %l2


.global halt_trap
halt_trap:
  !      flush

 !       set   boot_start, %g1
 !       set   boot_start+0x4, %g2
        jmp   %g1
        nop

trap_dispatcher:
	! store previous nPC as PC
	mov		%l2, %l1
	! store previous nPC+4 as nPC
	add     %l2, 4, %l2

/**/
irq_dispatcher:
	mov		%psr, %l4
	and		%l4, 0x1F, %l4
	mov		%wim, %l7
	srl		%l7, %l4, %l7
	and		%l7, 0x1, %l7
	cmp		%l7, 0x1			! test WIM for possible window overflow

	bne		irq_overflow_test_ok
	 nop

		! handle window overflow
		mov  %wim, %l5			! Calculate next WIM
		mov  %g1, %l7
		srl  %l5, 1, %g1
		sll  %l5, CORE_NWINDOWS-1 , %l4
		or   %l4, %g1, %g1

		save					! Get into window to be saved.
		mov  %g1, %wim
		std	%l0, [%sp + 0x00];
		std	%l2, [%sp + 0x08];
		std	%l4, [%sp + 0x10];
		std	%l6, [%sp + 0x18];
		std	%i0, [%sp + 0x20];
		std	%i2, [%sp + 0x28];
		std	%i4, [%sp + 0x30];
		std	%i6, [%sp + 0x38];
		restore					! Go back to trap window.
		mov		%l7, %g1

	irq_overflow_test_ok:
	add		%fp, -SA(KERNEL_FRAME), %sp

	srl		%l3, 4, %l5
	and 	%l5, 0xFF, %o0		! pass TT as parameter

		mov		%psr, %l5
		or		%l5, 0x20, %l5	! set PSR.ET (enable traps)
		mov		%l5, %psr

			call	dispatch_trap
			 nop

		mov		%psr, %l5
		andn	%l5, 0x20, %l5	! clear PSR.ET (disable traps)
		mov		%l5, %psr

	! handle possible window underflow when executing RETT
	mov		%psr, %l4
	and		%l4, 0x1F, %l4		! calculate current CWP
	mov		%wim, %l7
	srl		%l7, 1, %l5
	sll		%l7, CORE_NWINDOWS-1 , %l6
	or		%l5, %l6, %l7
	srl		%l7, %l4, %l7
	and		%l7, 0x1, %l7
	cmp		%l7, 0x1			! test WIM value after RETT for window underflow

	bne		irq_underflow_test_ok
	 nop

		! handle window underflow
		mov		%wim, %l3                  ! Calculate next WIM
		sll		%l3, 1, %l4
		srl		%l3, CORE_NWINDOWS-1, %l5
		or		%l5, %l4, %l5
		mov		%l5, %wim
		nop; nop; nop			! wait for delayed-write

		restore					! get into the window to be restore
		ldd	[%sp + 0x00], %l0;
		ldd	[%sp + 0x08], %l2;
		ldd	[%sp + 0x10], %l4;
		ldd	[%sp + 0x18], %l6;
		ldd	[%sp + 0x20], %i0;
		ldd	[%sp + 0x28], %i2;
		ldd	[%sp + 0x30], %i4;
		ldd	[%sp + 0x38], %i6;
		save					! Get back to the trap window.

	irq_underflow_test_ok:
return_from_trap:
    ! restore PIL&ICC fields of old PSR (in %l0)
	mov		%psr, %l4
	set		0xF00F00, %l3
	andn	%l4, %l3, %l4
	and		%l0, %l3, %l0
	or		%l4, %l0, %l4
	mov		%l4, %psr
	nop; nop; nop			! wait for PSR delayed-write

	jmp		%l1		! old PC
	 rett	%l2		! old nPC

bad_trap_dispatcher:
	mov		%tbr, %l3
	srl		%l3, 4, %l5
	and 	%l5, 0xFF, %o0		! pass TT as parameter
	mov		%l1, %o1
	mov		%l2, %o2
	mov		%psr, %o3
	mov		%psr, %l5
	or		%l5, 0x20, %l5	! set PSR.ET (enable traps)
	or		%l5, 0xF00, %l5
	mov		%l5, %psr
	call	dispatch_bad_trap
	 nop

        .global start_child_process
start_child_process:
        save  %sp, -112, %sp
        st    %i0, [%fp+0x44]

        set   chproc_started, %o1
        set   TRUE, %o2
        st    %o2, [%o1]

        set   chproc_fp, %o1
        st    %fp, [%o1]

        ld    [%fp+0x44], %l0
        call  %l0
         nop
 _sys_run_chproc_ret:
        set   chproc_started, %o1
        set   FALSE, %o2
        st    %o2, [%o1]
        set   chproc_ret_val, %o3
        st    %o0, [%o3]
        ret
         restore
! end of 'run_test'


sys_chproc_do_abort:
        set   chproc_fp, %g1
        ld    [%g1], %g2

 _sys_restore_loop_start:
        cmp   %fp, %g2
        be     _sys_restore_loop_end
         nop
        restore

        b     _sys_restore_loop_start
         nop
 _sys_restore_loop_end:
        b     _sys_run_chproc_ret
         nop
! end of 'chproc_abort'

srmmu_fault:
	ta  0
	nop
	nop
	nop
	jmp  %l1
	rett  %l2

.align  32
