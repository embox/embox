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
}pt_regs_t;

#endif /* MB_PTRACE_H_ */
