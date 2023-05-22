/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.05.23
 */
#include <stddef.h>
#include <stdint.h>

#include <asm/ptrace.h>
#include <asm/hal/env/traps_core.h>

struct gdb_regs {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t eip;
	uint32_t eflags;
};

extern __trap_handler __exception_table[0x20];

extern void gdb_stub(void *regs);

static int handle_breakpoint(uint32_t nr, void *data) {
	struct gdb_regs regs;
	
	((pt_regs_t *)data)->eip -= 1;
	
	regs.eax = ((pt_regs_t *)data)->eax;
	regs.ecx = ((pt_regs_t *)data)->ecx;
	regs.edx = ((pt_regs_t *)data)->edx;
	regs.ebx = ((pt_regs_t *)data)->ebx;
	regs.esp = ((pt_regs_t *)data)->esp;
	regs.ebp = ((pt_regs_t *)data)->ebp;
	regs.esi = ((pt_regs_t *)data)->esi;
	regs.edi = ((pt_regs_t *)data)->edi;
	regs.eip = ((pt_regs_t *)data)->eip;
	regs.eflags = ((pt_regs_t *)data)->eflags;

	gdb_stub(&regs);

	// __asm__ __volatile__("WBINVD");
	
	return 0;
}

void gdb_prepare_arch(void) {
	__exception_table[0x3] = handle_breakpoint;
}

void gdb_cleanup_arch(void) {
	__exception_table[0x3] = NULL;
}
