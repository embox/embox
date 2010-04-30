/**
 * @file
 *
 * @brief Microblaze specific definitions of trap processing
 *
 * @date 30.04.2010
 * @author Anton Bondarev
 */

#ifndef MB_PTRACE_H_
#define MB_PTRACE_H_

#include <types.h>

struct pt_regs {
	/*stack*/
	uint32_t regs[32];
	uint32_t msr;
	/*other information*/
	uint32_t pc;
}pt_regs_t;

#endif /* MB_PTRACE_H_ */
