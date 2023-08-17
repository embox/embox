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

extern void gdb_pack_str(struct gdb_packet *pkt, const char *str);
extern void gdb_pack_mem(struct gdb_packet *pkt, const void *mem, size_t nbyte);
extern void gdb_pack_begin(struct gdb_packet *pkt);
extern void gdb_pack_end(struct gdb_packet *pkt);
extern bool gdb_check_mem(uintptr_t addr);

static struct gdbstub_env __env;

static void gdb_read_gen_regs_cmd(struct gdb_packet *pkt) {
	unsigned long regval;
	size_t regsize;
	int regnum;

	for (regnum = 0;; regnum++) {
		regsize = gdb_read_reg(__env.regs, regnum, &regval);
		if (!regsize) {
			break;
		}
		gdb_pack_mem(pkt, &regval, regsize);
	}
}

static void gdb_read_reg_cmd(struct gdb_packet *pkt) {
	unsigned long regval;
	size_t regsize;
	int regnum;

	regnum = strtoul(pkt->buf + 2, NULL, 16);
	regsize = gdb_read_reg(__env.regs, regnum, &regval);

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
	bool (*fn)(int, void *, int);
	char *endptr;
	void *addr;
	int type;
	int kind;
	bool res;

	type = pkt->buf[2] - '0';
	addr = (void *)strtoul(pkt->buf + 4, &endptr, 16);
	kind = strtoul(endptr + 1, NULL, 16);
	fn = (pkt->buf[1] == 'Z') ? gdb_set_bpt : gdb_remove_bpt;

	res = fn(type, addr, kind);
	if (!res && (type == GDB_BPT_TYPE_SOFT)) {
		res = fn(GDB_BPT_TYPE_HARD, addr, kind);
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

int gdb_process_packet(struct gdb_packet *pkt) {
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
		gdb_read_gen_regs_cmd(pkt);
		break;

	case 'H': /* set thread */
		gdb_pack_str(pkt, "OK");
		break;

	case 'm': /* read memory */
		gdb_read_mem_cmd(pkt);
		break;

	case 'p': /* read register */
		gdb_read_reg_cmd(pkt);
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

void gdbstub_env_save(struct gdbstub_env *env) {
	memcpy(env, &__env, sizeof(struct gdbstub_env));
}

void gdbstub_env_restore(struct gdbstub_env *env) {
	memcpy(&__env, env, sizeof(struct gdbstub_env));
}
