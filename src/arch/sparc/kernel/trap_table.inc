
/* Reserved by SPARC architecture traps */
b boot_start; nop; nop; nop;  ! 00 reset trap
SRMMU_TFAULT;                 ! 01 instruction_access_exception
BAD_TRAP;                     ! 02 illegal_instruction
BAD_TRAP;                     ! 03 priveleged_instruction
BAD_TRAP;                     ! 04 fp_disabled
WOF_TRAP;                     ! 05 window_overflow
WUF_TRAP;                     ! 06 window_underflow
BAD_TRAP;                     ! 07 memory_address_not_aligned
BAD_TRAP;                     ! 08 fp_exception
SRMMU_DFAULT;                 ! 09 data_access_exception
BAD_TRAP;                     ! 0A tag_overflow
BAD_TRAP;                     ! 0B watchpoint_exception
BAD_TRAP;                     ! 0C undefined
BAD_TRAP;                     ! 0D undefined
BAD_TRAP;                     ! 0E undefined
BAD_TRAP;                     ! 0F undefined
BAD_TRAP;                     ! 10 undefined

/* Interrupt entries */
TRAP_ENTRY_INTERRUPT(0x1);         ! 11 interrupt level  1 - IRQ_AHB_error
TRAP_ENTRY_INTERRUPT(0x2);         ! 12 interrupt level  2 - IRQ_UART2
TRAP_ENTRY_INTERRUPT(0x3);         ! 13 interrupt level  3 - IRQ_UART1
TRAP_ENTRY_INTERRUPT(0x4);         ! 14 interrupt level  4
TRAP_ENTRY_INTERRUPT(0x5);         ! 15 interrupt level  5
TRAP_ENTRY_INTERRUPT(0x6);         ! 16 interrupt level  6
TRAP_ENTRY_INTERRUPT(0x7);         ! 17 interrupt level  7
TRAP_ENTRY_INTERRUPT(0x8);         ! 18 interrupt level  8 - IRQ_Timer1
TRAP_ENTRY_INTERRUPT(0x9);         ! 19 interrupt level  9 - IRQ_Timer2
TRAP_ENTRY_INTERRUPT(0xa);         ! 1A interrupt level 10
TRAP_ENTRY_INTERRUPT(0xb);         ! 1B interrupt level 11 - IRQ_DSU
TRAP_ENTRY_INTERRUPT(0xc);         ! 1C interrupt level 12
TRAP_ENTRY_INTERRUPT(0xd);         ! 1D interrupt level 13
TRAP_ENTRY_INTERRUPT(0xe);         ! 1E interrupt level 14 - IRQ_PCI
BAD_TRAP;                          ! 1F interrupt level 15 - IRQ_Halt (NMI)

/* Hardware traps. */
WEAK(0x20); WEAK(0x21); WEAK(0x22); WEAK(0x23); ! 20 - 23 undefined
WEAK(0x24);                                     ! 24 cp_disabled
            WEAK(0x25); WEAK(0x26); WEAK(0x27); ! 25 - 27 undefined
WEAK(0x28);                                     ! 28 cp_exception
            WEAK(0x29); WEAK(0x2A);             ! 29 - 2A undefined
                                    WEAK(0x2B); ! 2B data_store_error
WEAK(0x2C); WEAK(0x2D); WEAK(0x2E); WEAK(0x2F); ! 2C - 2F undefined
WEAK(0x30); WEAK(0x31); WEAK(0x32); WEAK(0x33); ! 30 - 33 undefined
WEAK(0x34); WEAK(0x35); WEAK(0x36); WEAK(0x37); ! 34 - 37 undefined
WEAK(0x38); WEAK(0x39); WEAK(0x3A); WEAK(0x3B); ! 38 - 3B undefined
WEAK(0x3C); WEAK(0x3D); WEAK(0x3E); WEAK(0x3F); ! 3C - 3F undefined
WEAK(0x40); WEAK(0x41); WEAK(0x42); WEAK(0x43); ! 40 - 43 undefined
WEAK(0x44); WEAK(0x45); WEAK(0x46); WEAK(0x47); ! 44 - 47 undefined
WEAK(0x48); WEAK(0x49); WEAK(0x4A); WEAK(0x4B); ! 48 - 4B undefined
WEAK(0x4C); WEAK(0x4D); WEAK(0x4E); WEAK(0x4F); ! 4C - 4F undefined
WEAK(0x50); WEAK(0x51); WEAK(0x52); WEAK(0x53); ! 50 - 53 undefined
WEAK(0x54); WEAK(0x55); WEAK(0x56); WEAK(0x57); ! 54 - 57 undefined
WEAK(0x58); WEAK(0x59); WEAK(0x5A); WEAK(0x5B); ! 58 - 5B undefined
WEAK(0x5C); WEAK(0x5D); WEAK(0x5E); WEAK(0x5F); ! 5C - 5F undefined
WEAK(0x60); WEAK(0x61); WEAK(0x62); WEAK(0x63); ! 60 - 63 undefined
WEAK(0x64); WEAK(0x65); WEAK(0x66); WEAK(0x67); ! 64 - 67 undefined
WEAK(0x68); WEAK(0x69); WEAK(0x6A); WEAK(0x6B); ! 68 - 6B undefined
WEAK(0x6C); WEAK(0x6D); WEAK(0x6E); WEAK(0x6F); ! 6C - 6F undefined
WEAK(0x70); WEAK(0x71); WEAK(0x72); WEAK(0x73); ! 70 - 73 undefined
WEAK(0x74); WEAK(0x75); WEAK(0x76); WEAK(0x77); ! 74 - 77 undefined
WEAK(0x78); WEAK(0x79); WEAK(0x7A); WEAK(0x7B); ! 78 - 7B undefined
WEAK(0x7C); WEAK(0x7D); WEAK(0x7E); WEAK(0x7F); ! 7C - 7F undefined

/* Software traps */
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;              ! 80 - 82
                                    SOFT_TRAP;  ! 83 TODO flush windows -- Eldar
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! 84 - 87
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! 88 - 8B
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! 8C - 8F
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! 90 - 93
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! 94 - 97
SOFT_TRAP;                                      ! 98
            EMBOX_SYSTEM_CALL;                  ! 99 embox system call
                        SOFT_TRAP;  SOFT_TRAP;  ! 9A - 9B
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! 9C - 9F
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! A0 - A3
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! A4 - A7
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! A8 - AB
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! AC - AF
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! B0 - B3
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! B4 - B7
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! B8 - BB
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! BC - BF
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! C0 - C3
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! C4 - C7
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! C8 - CB
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! CC - CF
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! D0 - D3
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! D4 - D7
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! D8 - DB
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! DC - DF
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! E0 - E3
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! E4 - E7
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! E8 - EB
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! EC - EF
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! F0 - F3
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! F4 - F7
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! F8 - FB
SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  SOFT_TRAP;  ! FC - FF
