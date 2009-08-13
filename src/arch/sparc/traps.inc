#include "asm/traps.h"

        .global _trap_table     !
        .global _hardreset      !

_trap_table:
_hardreset:

  TRAPL(boot_start);            ! 00 reset trap
  SRMMU_TFAULT;                 ! 01 instruction_access_exception
  BAD_TRAP;                     ! 02 illegal_instruction
  BAD_TRAP;                     ! 03 priveleged_instruction
  BAD_TRAP;                     ! 04 fp_disabled
  TRAP(window_overflow);        ! 05 window_overflow
  TRAP(window_underflow);       ! 06 window_underflow
  TRAP(trap_dispatcher);        ! 07 memory_address_not_aligned
  BAD_TRAP;                     ! 08 fp_exception
  SRMMU_DFAULT                  ! 09 data_access_exception
  BAD_TRAP;                     ! 0A tag_overflow

  BAD_TRAP;                     ! 0B watchpoint_exception
  BAD_TRAP;                     ! 0C undefined
  BAD_TRAP;                     ! 0D undefined
  BAD_TRAP;                     ! 0E undefined
  BAD_TRAP;                     ! 0F undefined
  BAD_TRAP;                     ! 10 undefined

  /* Interrupt entries */
  BAD_TRAP;                     ! 11 interrupt level  1   ! IRQ_AHB_error
  TRAP(irq_dispatcher);         ! 12 interrupt level  2   ! IRQ_UART2
  TRAP(irq_dispatcher);         ! 13 interrupt level  3   ! IRQ_UART1
  BAD_TRAP;                     ! 14 interrupt level  4   !
  TRAP(irq_dispatcher);         ! 15 interrupt level  5   ! from_dsp
  BAD_TRAP;                     ! 16 interrupt level  6   !
  TRAP(irq_dispatcher);         ! 17 interrupt level  7   ! IRQ_DMA
  TRAP(irq_dispatcher);         ! 18 interrupt level  8   ! IRQ_Timer1
  TRAP(irq_dispatcher);         ! 19 interrupt level  9   ! IRQ_Timer2
  BAD_TRAP;                     ! 1A interrupt level 10   ! IRQ_SecIRQ
  BAD_TRAP;                     ! 1B interrupt level 11   ! IRQ_DSU
  TRAP(irq_dispatcher);         ! 1C interrupt level 12   !
  TRAP(irq_dispatcher);         ! 1D interrupt level 13   !
  TRAP(irq_dispatcher);         ! 1E interrupt level 14   ! IRQ_PCI
  BAD_TRAP; !TRAP(halt_trap);              ! 1F interrupt level 15   ! IRQ_Halt (NMI)


  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 20 - 23 undefined
  BAD_TRAP;                                     ! 24 cp_disabled
  BAD_TRAP; BAD_TRAP; BAD_TRAP;                 ! 25 - 27 undefined
  BAD_TRAP;                                     ! 28 cp_exception
  BAD_TRAP; BAD_TRAP;                           ! 29 - 2A undefined
  BAD_TRAP;                                     ! 2B data_store_error
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 2C - 2F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 30 - 33 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 34 - 37 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 38 - 3B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 3C - 3F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 40 - 43 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 44 - 47 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 48 - 4B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 4C - 4F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 50 - 53 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 54 - 57 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 58 - 5B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 5C - 5F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 60 - 63 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 64 - 67 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 68 - 6B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 6C - 6F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 70 - 73 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 74 - 77 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 78 - 7B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 7C - 7F undefined

  /* Software traps */
  SOFT_TRAP;                                    ! 80
  SOFT_TRAP;                        ! 81
  SOFT_TRAP; SOFT_TRAP;                         ! 82 - 83
  SOFT_TRAP;                                    ! 84
  TRAP(cpu_context_save_trap);                  ! 85
  TRAP(cpu_context_restore_trap);               ! 86
  SOFT_TRAP;                                    ! 87
  SOFT_TRAP;                                    ! 88
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;  ! 89 - 8B
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 8C - 8F
  SOFT_TRAP; !TRAP(test_soft_traps_func);                   ! 90
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;              ! 91 - 93
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 94 - 97
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 98 - 9B
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 9C - 9F
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! A0 - A3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! A4 - A7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! A8 - AB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! AC - AF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! B0 - B3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! B4 - B7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! B8 - BB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! BC - BF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! C0 - C3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! C4 - C7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! C8 - CB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! CC - CF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! D0 - D3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! D4 - D7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! D8 - DB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! DC - DF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! E0 - E3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! E4 - E7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! E8 - EB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! EC - EF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! F0 - F3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! F4 - F7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! F8 - FB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! FC - FF
