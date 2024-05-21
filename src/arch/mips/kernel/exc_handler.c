/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.05.24
 */

#include <asm/ptrace.h>
#include <hal/cpu_idle.h>
#include <hal/ipl.h>
#include <util/log.h>

#define MIPS_EXC_INT  0  /* External Interrupt */
#define MIPS_EXC_ADEL 4  /* Address Error Exception (Load) */
#define MIPS_EXC_ADES 5  /* Address Error Exception (Store) */
#define MIPS_EXC_IBE  6  /* Instruction fetch Buss Error */
#define MIPS_EXC_DBE  7  /* Data load or store Buss Error */
#define MIPS_EXC_SYS  8  /* Syscall Exception */
#define MIPS_EXC_BP   9  /* Breakpoint Exception */
#define MIPS_EXC_RI   10 /* Reversed Instruction Exception */
#define MIPS_EXC_CPU  11 /* Coprocessor Unimplemented Exception */
#define MIPS_EXC_OV   12 /* Arithmetic Overflow Exception */
#define MIPS_EXC_TR   13 /* Trap Exception */
#define MIPS_EXC_FPE  14 /* Floating Point Exception */

extern void mips_interrupt_handler(void);

void mips_exception_handler(pt_regs_t *regs, unsigned long cause) {
	int i;

	switch (cause) {
	case MIPS_EXC_INT:
		mips_interrupt_handler();
		return;

	case MIPS_EXC_ADEL:
		log_info("Address Error Exception (Load)");
		break;

	case MIPS_EXC_ADES:
		log_info("Address Error Exception (Store)");
		break;

	case MIPS_EXC_IBE:
		log_info("Instruction fetch Buss Error");
		break;

	case MIPS_EXC_DBE:
		log_info("Data load or store Buss Error");
		break;

	case MIPS_EXC_RI:
		log_info("Reversed Instruction Exception");
		break;

	case MIPS_EXC_CPU:
		log_info("Coprocessor Unimplemented Exception");
		break;

	case MIPS_EXC_OV:
		log_info("Arithmetic Overflow Exception");
		break;

	case MIPS_EXC_FPE:
		log_info("Floating Point Exception");
		break;

	default:
		log_info("Unknown Exception");
		break;
	}

	log_info("gp (0x%lx); sp (0x%lx); fp(0x%lx)", regs->gp, regs->sp, regs->fp);
	log_info("ra (0x%lx); lo (0x%lx); hi(0x%lx)", regs->ra, regs->lo, regs->hi);
	log_info("cp0_status (0x%lx); pc (0x%lx);", regs->cp0_status, regs->pc);

	(void)i;
	for (i = 0; i < 25; i++) {
		log_info("reg[%d] = (0x%lx);\n", i, regs->reg[i]);
	}

	while (1) {
		arch_cpu_idle();
	}
}
