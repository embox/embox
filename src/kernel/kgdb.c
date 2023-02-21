/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 12.02.23
 */
#include <string.h>

#include <drivers/diag.h>
#include <embox/unit.h>
#include <embox/runlevel.h>
#include <debug/gdbstub.h>

EMBOX_UNIT_INIT(kgdb_init);

static struct gdbstub_state state;

static void get_packet(char *dst, size_t nbyte) {
	char ch;

	do {
		ch = diag_getc();
	} while (ch != '$');

	while (nbyte--) {
		ch = diag_getc();
		*dst++ = ch;
		if (ch == '#') {
			break;
		}
	}
}

static void put_packet(const char *src, size_t nbyte) {
	while (nbyte--) {
		diag_putc(*src++);
	}
}

static int kgdb_init(void) {
	state.connected = false;
	state.ops.get_packet = get_packet;
	state.ops.put_packet = put_packet;
	gdb_start_debugging(&state, mod_enable);
	return 0;
}
