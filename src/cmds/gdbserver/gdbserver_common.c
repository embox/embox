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
#include <debug/breakpoint.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/index_descriptor.h>

#include "gdbserver.h"

static int gdbfd;

static int default_bpt_type;

static bool connected;

static void gdbserver_bpt_handler(struct bpt_context *ctx) {
	struct gdb_packet pkt;
	ssize_t nread;
	int cmd;

	bpt_disable_all();

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
			bpt_remove_all();
			break;
		}

		gdbserver_write(gdbfd, "+", 1);

		cmd = gdb_process_packet(&pkt, ctx);

		if (cmd == GDB_CMD_CONT) {
			break;
		}

		if (cmd == GDB_CMD_DETACH) {
			bpt_remove_all();
			break;
		}

		gdbserver_write(gdbfd, pkt.buf, pkt.size);
	}

	ipl_disable();
	sched_unlock();

	bpt_remove(default_bpt_type, GDB_BPT_CTX_PC(ctx));
	bpt_enable_all();
}

void gdbserver_start(int fd, void *entry) {
	struct bpt_env env;

	assert(!connected);

	bpt_env_init(&env, gdbserver_bpt_handler, true);
	bpt_env_restore(&env);

	if (bpt_set(BPT_TYPE_SOFT, entry)) {
		default_bpt_type = BPT_TYPE_SOFT;
	}
	else {
		assert(bpt_set(BPT_TYPE_HARD, entry));
		default_bpt_type = BPT_TYPE_HARD;
	}

	gdbfd = fd;
}

void gdbserver_stop(void) {
	struct gdb_packet pkt;

	connected = false;

	bpt_disable_all();
	bpt_remove_all();

	gdb_process_cmd(GDB_CMD_TERM, &pkt);
	gdbserver_write(gdbfd, pkt.buf, pkt.size);
}
