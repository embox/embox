/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.06.23
 */
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <drivers/diag.h>
#include <kernel/printk.h>
#include <debug/gdbstub.h>

static int default_bpt_type;

static void kgdb_read(char *dst, size_t nbyte) {
	int i;

	do {
		dst[0] = diag_getc();
	} while (dst[0] != '$');

	for (i = 1; i < nbyte; i++) {
		dst[i] = diag_getc();
		if ((dst[i] == '#') && (i + 3 < nbyte)) {
			nbyte = i + 3;
		}
	}
}

static void kgdb_write(const char *src, size_t nbyte) {
	while (nbyte--) {
		diag_putc(*src++);
	}
}

static void kgdb_bpt_handler(struct gdb_regs *regs) {
	static bool connected = false;

	struct gdbstub_env env;
	struct gdb_packet pkt;
	int cmd;

	gdbstub_env_save(&env);
	env.regs = regs;
	gdbstub_env_restore(&env);

	gdb_deactivate_all_bpts();

	if (connected) {
		gdb_process_cmd(GDB_CMD_CONT, &pkt);
		kgdb_write(pkt.buf, pkt.size);
	}
	else {
		connected = true;
	}

	for (;;) {
		kgdb_read(pkt.buf, sizeof(pkt.buf));
		kgdb_write("+", 1);

		cmd = gdb_process_packet(&pkt);

		if (cmd == GDB_CMD_CONT) {
			break;
		}

		if (cmd == GDB_CMD_DETACH) {
			connected = false;
			gdb_remove_all_bpts();
			break;
		}

		kgdb_write(pkt.buf, pkt.size);
	}

	gdb_remove_bpt(default_bpt_type, GDB_REGS_PC(regs), 0);
	gdb_activate_all_bpts();
}

static bool kgdb_break_required(void) {
	while (diag_kbhit()) {
		if (diag_getc() == GDB_INTERRUPT_CHAR) {
			return true;
		}
	}

	return false;
}

void kgdb_break_pending(void *irq_ctx) {
	if (kgdb_break_required()) {
		assert(gdb_set_bpt(default_bpt_type, IRQ_CONTEXT_PC(irq_ctx), 0));
	}
}

void kgdb_start(void *entry) {
	printk("Remote debugging using diag\n");

	if (gdb_enable_bpts(GDB_BPT_TYPE_HARD)) {
		default_bpt_type = GDB_BPT_TYPE_HARD;
	}
	else {
		assert(gdb_enable_bpts(GDB_BPT_TYPE_SOFT));
		default_bpt_type = GDB_BPT_TYPE_SOFT;
	}

	gdb_set_handler(kgdb_bpt_handler);

	assert(gdb_set_bpt(default_bpt_type, entry, 0));
	gdb_activate_all_bpts();
}
