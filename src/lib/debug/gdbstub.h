/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 25.01.23
 */
#ifndef DEBUG_GDBSTUB_H_
#define DEBUG_GDBSTUB_H_

#define GDBSTUB_IMPL 1

#ifndef __ASSEMBLER__
#include <stdbool.h>

#include <config/embox/lib/debug/gdbstub.h>

#define MAX_BPTS OPTION_NUMBER_embox__lib__debug__gdbstub__max_bpts
#include <asm/gdb.h>

struct gdbstub_ops {
	void (*get_packet)(char *dst, size_t nbyte);
	void (*put_packet)(const char *src, size_t nbyte);
};

struct gdbstub_state {
	struct breakpoint bpts[MAX_BPTS];
	struct gdbstub_ops ops;
	bool connected;
};

extern void gdb_start_debugging(struct gdbstub_state *state, void *entry);
extern void gdb_stop_debugging(struct gdbstub_state *state);

extern void gdb_serial_stub(void *regs);
#endif /* !__ASSEMBLER__ */

#endif /* DEBUG_GDBSTUB_H_ */
