/**
 * @file
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_ASM_H_
#define MIPS_ASM_H_

#ifdef __ASSEMBLER__

#define PTR_ADD     add
#define PTR_ADDU    addu
#define PTR_ADDI    addi
#define PTR_ADDIU   addiu
#define PTR_SUB     sub
#define PTR_SUBU    subu
#define PTR_L       lw
#define PTR_S       sw
#define PTR_LA      la
#define PTR_LI      li
#define PTR_SLL     sll
#define PTR_SLLV    sllv
#define PTR_SRL     srl
#define PTR_SRLV    srlv
#define PTR_SRA     sra
#define PTR_SRAV    srav

#define LONG_ADD    add
#define LONG_ADDU   addu
#define LONG_ADDI   addi
#define LONG_ADDIU  addiu
#define LONG_SUB    sub
#define LONG_SUBU   subu
#define LONG_L      lw
#define LONG_S      sw
#define LONG_SLL    sll
#define LONG_SLLV   sllv
#define LONG_SRL    srl
#define LONG_SRLV   srlv
#define LONG_SRA    sra
#define LONG_SRAV   srav

#define LONG        .word

#endif

#endif /* MIPS_ASM_H_ */
