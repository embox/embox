/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <stdbool.h>

#include <debug/gdbstub.h>
#include <debug/breakpoint.h>

#include "utils.h"

static void gdb_read_gen_regs_cmd(struct gdb_packet *pkt,
    struct bpt_context *ctx) {
	unsigned long regval;
	size_t regsize;
	int regnum;

	for (regnum = 0;; regnum++) {
		regsize = gdb_read_reg(ctx, regnum, &regval);
		if (!regsize) {
			break;
		}
		gdb_pack_mem(pkt, &regval, regsize);
	}
}

static void gdb_read_reg_cmd(struct gdb_packet *pkt, struct bpt_context *ctx) {
	unsigned long regval;
	size_t regsize;
	int regnum;

	regnum = strtoul(pkt->buf + 2, NULL, 16);
	regsize = gdb_read_reg(ctx, regnum, &regval);

	if (regsize) {
		gdb_pack_mem(pkt, &regval, regsize);
	}
	else {
		gdb_pack_str(pkt, "E01");
	}
}

static void gdb_read_mem_cmd(struct gdb_packet *pkt) {
	char *endptr;
	uintptr_t addr;
	size_t len;

	addr = strtoul(pkt->buf + 2, &endptr, 16);

	if (addr && gdb_check_mem(addr)) {
		len = strtoul(endptr + 1, NULL, 16);
		gdb_pack_mem(pkt, (void *)addr, len);
	}
	else {
		gdb_pack_str(pkt, "E01");
	}
}

static void gdb_bpt_cmd(struct gdb_packet *pkt) {
	bool (*bpt_fn)(int, void *);
	void *addr;
	int type;
	bool res;

	type = pkt->buf[2] - '0';
	addr = (void *)strtoul(pkt->buf + 4, NULL, 16);
	bpt_fn = (pkt->buf[1] == 'Z') ? bpt_set : bpt_remove;

	res = bpt_fn(type, addr);
	if (!res && (type == BPT_TYPE_SOFT)) {
		res = bpt_fn(BPT_TYPE_HARD, addr);
	}

	if (res) {
		gdb_pack_str(pkt, "OK");
	}
	else {
		gdb_pack_str(pkt, "E01");
	}
}

static void gdb_query_cmd(struct gdb_packet *pkt) {
	char *request;

	request = pkt->buf + 2;

	switch (*request) {
	case 'A':
		if (!memcmp(request, "Attached", 8)) {
			gdb_pack_str(pkt, "1");
		}
		break;

	case 'C':
		if (request[1] == '#') {
			gdb_pack_str(pkt, "QC01");
		}
		break;

	case 'f':
		if (!memcmp(request, "fThreadInfo", 11)) {
			gdb_pack_str(pkt, "m01");
		}
		break;

	case 's':
		if (!memcmp(request, "sThreadInfo", 11)) {
			gdb_pack_str(pkt, "l");
		}
		break;

	case 'S':
		if (!memcmp(request, "Supported", 9)) {
			gdb_pack_str(pkt, "qXfer:features:read+;PacketSize=");
			sprintf(pkt->buf + pkt->size, "%lx", GDB_PACKET_SIZE);
			gdb_pack_str(pkt, pkt->buf + pkt->size);
		}
		else if (!memcmp(request, "Symbol", 6)) {
			gdb_pack_str(pkt, "OK");
		}
		break;

	case 'X':
		if (!memcmp(request, "Xfer:features:read", 18)) {
			gdb_pack_str(pkt, GDB_FEATURE_STR);
		}
		break;

	default:
		break;
	}
}

int gdb_process_packet(struct gdb_packet *pkt, struct bpt_context *ctx) {
	int cmd;

	cmd = GDB_CMD_OTHER;

	gdb_pack_begin(pkt);

	switch (pkt->buf[1]) {
	case 'c': /* continue */
	case 's': /* step */
		cmd = GDB_CMD_CONT;

	case '?': /* get status */
		gdb_pack_str(pkt, "S05");
		break;

	case 'D': /* detach */
	case 'k': /* kill */
		cmd = GDB_CMD_DETACH;
		break;

	case 'g': /* read general registers */
		gdb_read_gen_regs_cmd(pkt, ctx);
		break;

	case 'H': /* set thread */
		gdb_pack_str(pkt, "OK");
		break;

	case 'm': /* read memory */
		gdb_read_mem_cmd(pkt);
		break;

	case 'p': /* read register */
		gdb_read_reg_cmd(pkt, ctx);
		break;

	case 'q': /* query command */
		gdb_query_cmd(pkt);
		break;

	case 'T': /* thread status */
		gdb_pack_str(pkt, "OK");
		break;

	case 'Z': /* insert breakpoint */
	case 'z': /* remove breakpoint */
		gdb_bpt_cmd(pkt);
		break;

	default:
		break;
	}

	gdb_pack_end(pkt);

	return cmd;
}

void gdb_process_cmd(int cmd, struct gdb_packet *pkt) {
	gdb_pack_begin(pkt);

	switch (cmd) {
	case GDB_CMD_CONT:
		gdb_pack_str(pkt, "S05");
		break;

	case GDB_CMD_TERM:
		gdb_pack_str(pkt, "X0f");
		break;

	default:
		break;
	}

	gdb_pack_end(pkt);
}
