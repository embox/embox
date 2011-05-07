/**
 * @file
 * @brief Microblaze definition objects for setjmp
 *
 * @date 11.03.10
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_SETJMP_H_
#define MICROBLAZE_SETJMP_H_

#include <types.h>

#ifndef __ASSEMBLER__

typedef  uint32_t reg_t;

typedef struct {
	/*stack*/
	reg_t regs[32];
	reg_t pc;
	reg_t msr;
	reg_t ear;
	reg_t esr;
	reg_t fsr;
	int pt_mode;
} __jmp_buf[1];

#endif /* __ASSEMBLER__ */

#define JB_SP        0x00
#define JB_LP        0x04
#define JB_REGS      0x08

#define JB_SIZE     (4 * 15)

#endif /* MICROBLAZE_SETJMP_H_ */
