/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.06.23
 */
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <drivers/diag.h>
#include <kernel/printk.h>
#include <debug/gdbstub.h>

static struct gdb_arch gdb_arch;

static void read_packet(char *dst, size_t nbyte) {
	int i;

	do {
		dst[0] = diag_getc();
	} while (dst[0] != '$');

	for (i = 1; i < nbyte; i++) {
		dst[i] = diag_getc();
		if (dst[i] == '#') {
			break;
		}
	}
}

static void write_packet(const char *src, size_t nbyte) {
	while (nbyte--) {
		diag_putc(*src++);
	}
}

static void kgdb_handle_debug_excpt(struct gdb_regs *regs) {
	static bool connected = false;

	struct gdbstub_env env;

	env.arch = &gdb_arch;
	env.regs = regs;

	if (connected) {
		env.packet.buf[1] = '?';
		gdb_process_packet(&env);
		write_packet(env.packet.buf, env.packet.size);
	}
	else {
		gdb_arch.remove_all_bpts();
		connected = true;
	}

	for (;;) {
		read_packet(env.packet.buf, sizeof(env.packet.buf));
		write_packet("+", 1);

		gdb_process_packet(&env);

		if (env.cmd != GDBSTUB_SEND_REPLY) {
			if (env.cmd == GDBSTUB_DETACH) {
				env.arch->remove_all_bpts();
			}
			break;
		}

		write_packet(env.packet.buf, env.packet.size);
	}
}

void kgdb_init(void *entry) {
	printk("Remote debugging using diag\n");

	gdb_arch_prepare(kgdb_handle_debug_excpt);
	gdb_arch_init(&gdb_arch);
	gdb_arch.insert_bpt(entry, 0);
}
