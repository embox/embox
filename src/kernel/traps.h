#ifndef _TRAPS_H_
#define _TRAPS_H_

/* The traptable has to be the first code in a boot PROM. */

/* Entry for traps which jump to a programmer-specified trap handler.  */
#define TRAP(H)  mov %psr, %l0; sethi %hi(H), %l4; jmp %l4+%lo(H); mov %tbr, %l3;
#define TRAPL(H)  mov %g0, %l0; sethi %hi(H), %l4; jmp %l4+%lo(H); nop;
/*for mmu*/
#define SRMMU_TFAULT rd %psr, %l0; rd %wim, %l3; b srmmu_fault; mov 1, %l7;
#define SRMMU_DFAULT rd %psr, %l0; rd %wim, %l3; b srmmu_fault; mov 0, %l7;

/* Unexpected trap will halt the processor by forcing it to error state */
#define BAD_TRAP ta 0; nop; nop; nop;

/* Software trap. Treat as BAD_TRAP */
#define SOFT_TRAP BAD_TRAP

#endif // _TRAPS_H_
