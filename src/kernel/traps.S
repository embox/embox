#include "leon_config.h"

#include "traps.inc"



! skip instruction
skip:
        flush
        jmpl    %l2, %g0
        rett    %l2 + 4
        nop
        nop

irq_empty:

        jmpl %l1,%g0 ! old PC
        rett %l2 ! old nPC
        nop
        nop




  /* Window overflow trap handler. */
        .global window_overflow

window_overflow:

        mov  %wim, %l3                  ! Calculate next WIM
        mov  %g1, %l7
        srl  %l3, 1, %g1
        sll  %l3, CORE_NWINDOWS-1 , %l4
        or   %l4, %g1, %g1

        save                              ! Get into window to be saved.
        mov  %g1, %wim
        nop; nop; nop
        st   %l0, [%sp + 0];
        st   %l1, [%sp + 4];
        st   %l2, [%sp + 8];
        st   %l3, [%sp + 12];
        st   %l4, [%sp + 16];
        st   %l5, [%sp + 20];
        st   %l6, [%sp + 24];
        st   %l7, [%sp + 28];
        st   %i0, [%sp + 32];
        st   %i1, [%sp + 36];
        st   %i2, [%sp + 40];
        st   %i3, [%sp + 44];
        st   %i4, [%sp + 48];
        st   %i5, [%sp + 52];
        st   %i6, [%sp + 56];
        st   %i7, [%sp + 60];
        restore                         ! Go back to trap window.
        mov  %l7, %g1
        jmp  %l1                        ! Re-execute save.
        rett %l2

  /* Window underflow trap handler.  */

        .global  window_underflow

window_underflow:

        mov  %wim, %l3                  ! Calculate next WIM
        sll  %l3, 1, %l4
        srl  %l3, CORE_NWINDOWS-1, %l5
        or   %l5, %l4, %l5
        mov  %l5, %wim
        nop; nop; nop
        restore                         ! Two restores to get into the
        restore                         ! window to restore
        ld   [%sp + 0], %l0;            ! Restore window from the stack
        ld   [%sp + 4], %l1;
        ld   [%sp + 8], %l2;
        ld   [%sp + 12], %l3;
        ld   [%sp + 16], %l4;
        ld   [%sp + 20], %l5;
        ld   [%sp + 24], %l6;
        ld   [%sp + 28], %l7;
        ld   [%sp + 32], %i0;
        ld   [%sp + 36], %i1;
        ld   [%sp + 40], %i2;
        ld   [%sp + 44], %i3;
        ld   [%sp + 48], %i4;
        ld   [%sp + 52], %i5;
        ld   [%sp + 56], %i6;
        ld   [%sp + 60], %i7;
        save                            ! Get back to the trap window.
        save
        jmp  %l1                        ! Re-execute restore.
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



! вытеснение
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

! сохраняем признак будильника
rtc_trap:
        !set     io_rtc_alarm, %l4
        !set     0x01, %l5
        !stb     %l5, [%l4]

        jmpl    %l1, %g0
         rett    %l2


! выход в пульт по нажатию останов
.global halt_trap
halt_trap:
  !      flush

 !       set   boot_start, %g1
 !       set   boot_start+0x4, %g2
        jmp   %g1
        nop


! обработчик trap-ов через менеджер прерываний
trap_handler:
  ! if (irq_current < IRQ_BUFFER_SIZE)
        set     irq_current, %l3
       	ld      [%l3], %l4
       	cmp     %l4, IRQ_BUFFER_SIZE
       	bge     trap_handler_ret       ! if queue full, then skip irq
        nop
  ! {
        ! calculate position in buffer
        set     irq_current, %l3
        ld      [%l3], %l5
        ! irq_current++;
        add     %l5, 1, %l4
        st      %l4, [%l3]
        sll     %l5, 2, %l5

        ! store psr (irq_tbl_psr[irq_current] = psr) (%psr in %l0)
        mov     %l0, %l4
        set     0xF00F00, %l3
        and     %l4, %l3, %l4
        set     irq_tbl_psr, %l3
        st      %l4, [%l3+%l5]

        ! calculate trap type
        mov     %tbr, %l4
        srl     %l4, 4, %l4
        and     %l4, 0xFF, %l4
        ! store tbr.tt (irq_tbl_tt[irq_current] = tbr.tt)
        set     irq_tbl_tt, %l3
        st      %l4, [%l3+%l5]

        ! set new PIL (correct only for traps!!!)
        set     0xF00, %l3
        mov     %psr, %l4
        andn    %l4, 0xF00, %l4
        or      %l4, %l3, %l4
        mov     %l4, %psr

        ! store previous nPC as PC
        set     irq_tbl_pc, %l3
        st      %l2, [%l3+%l5]
        ! store previous nPC+4 as nPC
        set     irq_tbl_npc, %l3
        add     %l2, 4, %l2
        st      %l2, [%l3+%l5]

        ! run manager
        set     irq_manager_caller, %l2
        jmpl    %l2, %g0
         rett   %l2+4
  ! }
  ! else
  trap_handler_ret:
        jmpl    %l1, %g0
         rett   %l2

! обработчик прерываний через менеджер
irq_handler:
  ! if (irq_current < IRQ_BUFFER_SIZE)
        set     irq_current, %l3
       	ld      [%l3], %l4
       	cmp     %l4, IRQ_BUFFER_SIZE
       	bge     irq_handler_ret       ! if queue full, then skip irq
        nop
  ! {
        ! calculate position in buffer
        set     irq_current, %l3
        ld      [%l3], %l5
        ! irq_current++;
        add     %l5, 1, %l4
        st      %l4, [%l3]
        sll     %l5, 2, %l5

        ! store psr (irq_tbl_psr[irq_current] = psr) (%psr in %l0)
        mov     %l0, %l4
        set     0xF00F00, %l3
        and     %l4, %l3, %l4
        set     irq_tbl_psr, %l3
        st      %l4, [%l3+%l5]

        ! calculate trap type
        mov     %tbr, %l4
        srl     %l4, 4, %l4
        and     %l4, 0xFF, %l4
        ! store tbr.tt (irq_tbl_tt[irq_current] = tbr.tt)
        set     irq_tbl_tt, %l3
        st      %l4, [%l3+%l5]


        ! set new PIL (correct only for interrupts!!!)
        !set     0xF00, %l3
        sub     %l4, 0x10, %l3
        and     %l3, 0xF, %l3
        sll     %l3, 8, %l3

        mov     %psr, %l4
        andn    %l4, 0xF00, %l4
        or      %l4, %l3, %l4
        mov     %l4, %psr

        ! store previous PC
        set     irq_tbl_pc, %l3
        st      %l1, [%l3+%l5]
        ! store previous nPC
        set     irq_tbl_npc, %l3
        st      %l2, [%l3+%l5]

        ! run manager
        set     irq_manager_caller, %l2
        jmpl    %l2, %g0
         rett   %l2+4
  ! }
  ! else
  irq_handler_ret:
        jmpl    %l1, %g0
         rett   %l2

irq_manager_caller:
        save  %sp, -104, %sp

        call    irq_manager
        nop

        restore
        save  %sp, -104, %sp

        ! Simutale returning from trap
        mov     %psr, %g4
        or      %g4, 0x080, %g4    ! set PSR.S (supervisor mode)
        andn    %g4, 0x020, %g4    ! disable traps
        mov     %g4, %psr

        ! calculate position in buffer
        set     irq_current, %l3
        ld      [%l3], %l4
        add     %l4, -1, %l4
        ! irq_current--;
        st      %l4, [%l3]
        sll     %l4, 2, %l5
        ! restore PSR (PIL&ICC)
        mov     %psr, %l4
        set     0xF00F00, %l3
        andn    %l4, %l3, %l4
        set     irq_tbl_psr, %l3
        ld      [%l3+%l5], %l3
        or      %l4, %l3, %l4
        mov     %l4, %psr

        ! проверяем подтверждение прерывания процесса
        set     chproc_abort_accept, %g1
        ld      [%g1], %g2
        cmp     %g2, TRUE
        bne     _cont_program
        nop

        set     sys_chproc_do_abort, %g1
        add     %g1, 4, %g2

        jmpl    %g1, %g0
        rett   %g2

    _cont_program:
        ! restore PSR (PIL&ICC)
        mov     %psr, %l4
        set     0xF00F00, %l3
        andn    %l4, %l3, %l4
        set     irq_tbl_psr, %l3
        ld      [%l3+%l5], %l3
        or      %l4, %l3, %l4
        mov     %l4, %psr

        ! load ret PC
        set     irq_tbl_pc, %l3
        ld      [%l3+%l5], %g1

        ! load ret nPC
        set     irq_tbl_npc, %l3
        ld      [%l3+%l5], %g2

        jmpl    %g1, %g0
         rett   %g2





        .global start_child_process
start_child_process:
        save  %sp, -112, %sp
        st    %i0, [%fp+0x44]

        set   chproc_started, %o1
        set   TRUE, %o2
        st    %o2, [%o1]

        ! сохраняем frame pointer
        set   chproc_fp, %o1
        st    %fp, [%o1]

        ! вызываем указанный тест
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

 ! цикл отката по стэку
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
