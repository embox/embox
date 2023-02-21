/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>
#include <stdbool.h>

#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/sched/sched_lock.h>

#include <util/macro.h>
#include <hal/context.h>
#include <drivers/diag.h>
#include <debug/gdbstub.h>

#define HEXCHARS "0123456789abcdef"

extern char _stack_top;

static struct gdbstub_state *curr_state;

static char *pack_str(char *dst, const char *src, uint8_t *checksum) {
	uint8_t sum;
	char ch;

	sum = 0;

	while ((ch = *src++)) {
		sum += ch;
		*dst++ = ch;
	}
	if (checksum) {
		*checksum += sum;
	}
	return dst;
}

static char *
pack_mem(char *dst, const void *src, size_t nbyte, uint8_t *checksum) {
	uint8_t *ptr;
	uint8_t sum;

	sum = 0;

	for (ptr = (uint8_t *)src; nbyte--; ptr++) {
		*dst = HEXCHARS[*ptr >> 4];
		sum += *dst++;
		*dst = HEXCHARS[*ptr & 0xf];
		sum += *dst++;
	}
	if (checksum) {
		*checksum += sum;
	}
	return dst;
}

void gdb_start_debugging(struct gdbstub_state *state, void *entry) {
	memset(state->bpts, 0, sizeof(state->bpts));
	gdb_set_breakpoint(entry, gdb_find_breakpoint(NULL, state->bpts));
	curr_state = state;
}

void gdb_stop_debugging(struct gdbstub_state *state) {
	gdb_remove_all_breakpoints(state->bpts);
}

void gdb_serial_stub(void *regs) {
	char buf[PACKET_SIZE + 4];
	struct breakpoint *bpt;
	struct context *ctx;
	struct thread *curr;
	struct thread *th;
	struct task *task;
	char *request;
	char *buf_ptr;
	void *addr;
	uint32_t num;
	uint8_t checksum;

	sched_lock();

	ctx = regs;
	curr = thread_self();

	if (!curr_state->connected) {
		curr_state->connected = true;
		gdb_remove_all_breakpoints(curr_state->bpts);
		goto get_request;
	}

	buf[0] = '?';

	for (;;) {
		checksum = 0;
		request = buf;
		buf_ptr = buf + 1;

		switch (*request) {
		case '?':
			num = htobe32((curr) ? curr->id : 1);
			buf_ptr = pack_str(buf_ptr, "T05thread:", &checksum);
			buf_ptr = pack_mem(buf_ptr, &num, sizeof(num), &checksum);
			buf_ptr = pack_str(buf_ptr, ";", &checksum);
			break;

		case 'D':
			gdb_remove_all_breakpoints(curr_state->bpts);
		case 'c':
			goto cont;

		case 'g':
			for (num = 0; gdb_reg(ctx, num); num++) {
				buf_ptr = pack_mem(buf_ptr, gdb_reg(ctx, num), gdb_reg_sz(num),
				                   &checksum);
			}
			break;

		case 'H':
			if (curr && (request[1] == 'g')) {
				num = strtoul(request + 2, &request, 16);
				task_foreach(task) {
					task_foreach_thread(th, task) {
						if (num == th->id) {
							ctx = (th == thread_self()) ? regs : &th->context;
							curr = th;
						}
					}
				}
			}
		case 'T':
			buf_ptr = pack_str(buf_ptr, "OK", &checksum);
			break;

		case 'm':
			addr = (void *)strtoul(request + 1, &request, 16);
			if ((curr && (addr < curr->stack.stack + curr->stack.stack_sz))
			    || (!curr && (addr < (void *)&_stack_top))) {
				num = strtoul(request + 1, NULL, 16);
				buf_ptr = pack_mem(buf_ptr, addr, num, &checksum);
			}
			else {
				buf_ptr = pack_str(buf_ptr, "E01", &checksum);
			}
			break;

		case 'p':
			num = strtoul(request + 1, NULL, 16);
			if (gdb_reg(ctx, num)) {
				buf_ptr = pack_mem(buf_ptr, gdb_reg(ctx, num), gdb_reg_sz(num),
				                   &checksum);
			}
			else {
				buf_ptr = pack_str(buf_ptr, "E01", &checksum);
			}
			break;

		case 'q':
			request += 1;
			switch (*request) {
			case 'A':
				if (!memcmp(request, "Attached", 8)) {
					buf_ptr = pack_str(buf_ptr, "1", &checksum);
				}
				break;

			case 'f':
				if (!memcmp(request, "fThreadInfo", 11)) {
					buf_ptr = pack_str(buf_ptr, "m", &checksum);
					if (curr) {
						task_foreach(task) {
							task_foreach_thread(th, task) {
								num = htobe32(th->id);
								buf_ptr = pack_mem(buf_ptr, &num, sizeof(num),
								                   &checksum);
								buf_ptr = pack_str(buf_ptr, ",", &checksum);
							}
						}
					}
					else {
						num = htobe32(1);
						buf_ptr = pack_mem(buf_ptr, &num, sizeof(num),
						                   &checksum);
					}
				}
				break;

			case 's':
				if (!memcmp(request, "sThreadInfo", 11)) {
					buf_ptr = pack_str(buf_ptr, "l", &checksum);
				}
				break;

			case 'S':
				if (!memcmp(request, "Supported", 9)) {
					buf_ptr = pack_str(buf_ptr,
					                   "qXfer:features:read+;"
					                   "PacketSize=" MACRO_STRING(PACKET_SIZE),
					                   &checksum);
				}
				else if (!memcmp(request, "Symbol", 6)) {
					buf_ptr = pack_str(buf_ptr, "OK", &checksum);
				}
				break;

			case 'X':
				if (!memcmp(request, "Xfer:features:read", 18)) {
					buf_ptr = pack_str(buf_ptr, FEATURE_STR, &checksum);
				}
				break;
			}
			break;

		case 'z':
			if (request[1] == '0') {
				addr = (void *)strtoul(request + 3, NULL, 16);
				bpt = gdb_find_breakpoint(addr, curr_state->bpts);
				if (bpt) {
					gdb_remove_breakpoint(bpt);
				}
				buf_ptr = pack_str(buf_ptr, "OK", &checksum);
			}
			break;

		case 'Z':
			if (request[1] == '0') {
				addr = (void *)strtoul(request + 3, NULL, 16);
				bpt = gdb_find_breakpoint(NULL, curr_state->bpts);
				if (bpt) {
					gdb_set_breakpoint(addr, bpt);
					buf_ptr = pack_str(buf_ptr, "OK", &checksum);
				}
				else {
					buf_ptr = pack_str(buf_ptr, "E01", &checksum);
				}
			}
			break;
		}

		buf[0] = '$';
		*buf_ptr++ = '#';
		buf_ptr = pack_mem(buf_ptr, &checksum, 1, NULL);
		curr_state->ops.put_packet(buf, buf_ptr - buf);

get_request:
		curr_state->ops.get_packet(buf, sizeof(buf));
		curr_state->ops.put_packet("+", 1);
	}

cont:
	sched_unlock();
}
