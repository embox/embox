/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 14.08.23
 */
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/poll.h>
#include <sys/types.h>

#include <hal/ipl.h>
#include <hal/arch.h>
#include <debug/gdbstub.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/index_descriptor.h>

#include "gdbserver.h"

static int gdbfd;

static int default_bpt_type;

static bool connected;

static void gdbserver_bpt_handler(struct gdb_regs *regs) {
	struct gdbstub_env env;
	struct gdb_packet pkt;
	ssize_t nread;
	int cmd;

	gdbstub_env_save(&env);
	env.regs = regs;
	gdbstub_env_restore(&env);

	gdb_deactivate_all_bpts();

	sched_lock();
	ipl_enable();

	if (connected) {
		gdb_process_cmd(GDB_CMD_CONT, &pkt);
		gdbserver_write(gdbfd, pkt.buf, pkt.size);
	}
	else {
		connected = true;
	}

	for (;;) {
		nread = gdbserver_read(gdbfd, pkt.buf, sizeof(pkt.buf));

		if (nread <= 0) {
			gdb_remove_all_bpts();
			break;
		}

		gdbserver_write(gdbfd, "+", 1);

		cmd = gdb_process_packet(&pkt);

		if (cmd == GDB_CMD_CONT) {
			break;
		}

		if (cmd == GDB_CMD_DETACH) {
			gdb_remove_all_bpts();
			break;
		}

		gdbserver_write(gdbfd, pkt.buf, pkt.size);
	}

	ipl_disable();
	sched_unlock();

	gdb_remove_bpt(default_bpt_type, GDB_REGS_PC(regs), 0);
	gdb_activate_all_bpts();
}

void gdbserver_start(int fd, void *entry) {
	assert(!connected);

	if (gdb_enable_bpts(GDB_BPT_TYPE_SOFT)) {
		default_bpt_type = GDB_BPT_TYPE_SOFT;
	}
	else {
		assert(gdb_enable_bpts(GDB_BPT_TYPE_HARD));
		default_bpt_type = GDB_BPT_TYPE_HARD;
	}

	gdbfd = fd;

	gdb_set_handler(gdbserver_bpt_handler);
	assert(gdb_set_bpt(default_bpt_type, entry, 0));
}

void gdbserver_stop(void) {
	struct gdb_packet pkt;

	connected = false;

	gdb_remove_all_bpts();
	gdb_disable_bpts(default_bpt_type);

	gdb_process_cmd(GDB_CMD_TERM, &pkt);
	gdbserver_write(gdbfd, pkt.buf, pkt.size);
}
