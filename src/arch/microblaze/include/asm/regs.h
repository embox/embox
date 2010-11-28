/*
 * @file
 * @brief aliases for register's name mentioned in microblaze API
 *
 * @date 17.11.2009
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_REGS_H_
#define MICROBLAZE_REGS_H_

#define reg_zero   r0    /**< zero constant register*/
#define reg_sp     r1    /**< stack pointer register*/
#define reg_tmp1   r11   /**< temporary register*/
#define reg_tmp2   r12   /**< temporary register*/
#define reg_ireta  r14   /**< return address from interrupt*/
#define reg_sreta  r15   /**< return address from subroutine*/
#define reg_treta  r16   /**< return address from trap*/
#define reg_ereta  r17   /**< return address from exception*/

#define REGS_INIT() \
	addi r2, reg_zero, 0;  \
	addi r5, reg_zero, 0;  \
	addi r6, reg_zero, 0;  \
	addi r7, reg_zero, 0;  \
	addi r8, reg_zero, 0;  \
	addi r9, reg_zero, 0;  \
	addi r10, reg_zero, 0; \
	addi r11, reg_zero, 0; \
	addi r12, reg_zero, 0; \
	addi r13, reg_zero, 0; \
	addi r14, reg_zero, 0; \
	addi r15, reg_zero, 0; \
	addi r18, reg_zero, 0; \
	addi r19, reg_zero, 0; \
	addi r20, reg_zero, 0; \
	addi r21, reg_zero, 0; \
	addi r22, reg_zero, 0; \
	addi r23, reg_zero, 0; \
	addi r24, reg_zero, 0; \
	addi r25, reg_zero, 0; \
	addi r26, reg_zero, 0; \
	addi r27, reg_zero, 0; \
	addi r28, reg_zero, 0; \
	addi r29, reg_zero, 0; \
	addi r30, reg_zero, 0; \
	addi r31, reg_zero, 0;


#endif /* MICROBLAZE_REGS_H_ */
