/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <util/macro.h>
#include <kernel/thread.h>

#include "gdb_priv.h"

static void handle_read_mem(struct gdb_packet *pkt) {
	extern char _stack_top;

	struct thread *th;
	char *endptr;
	void *addr;
	size_t len;

	th = thread_self();

	addr = (void *)strtoul(&pkt->buf[2], &endptr, 16);
	if ((th && (addr < th->stack.stack + th->stack.stack_sz)) ||
	    (!th && (addr < (void *)&_stack_top))) {
		len = strtoul(endptr + 1, NULL, 16);
		gdb_pack_mem(pkt, addr, len);
	}
	else {
		gdb_pack_str(pkt, "E01");
	}
}

static void handle_get_reg(struct gdb_packet *pkt, void *regs) {
	unsigned long regval;
	unsigned int regnum;
	size_t regsize;

	regnum = strtoul(&pkt->buf[2], NULL, 16);
	if ((regsize = gdb_get_reg(regs, regnum, &regval))) {
		gdb_pack_mem(pkt, &regval, regsize);
	}
	else {
		gdb_pack_str(pkt, "E01");
	}
}

static void handle_set_remove_bpt(struct gdb_packet *pkt, int (*fn)(void *)) {
	void *addr;

	if (pkt->buf[2] == '0') {
		addr = (void *)strtoul(&pkt->buf[4], NULL, 16);
		if (fn(addr)) {
			gdb_pack_str(pkt, "OK");
		}
		else {
			gdb_pack_str(pkt, "E01");
		}
	}
}

static void handle_gen_query(struct gdb_packet *pkt) {
	char *request;

	request = &pkt->buf[2];

	switch (*request) {
	case 'A':
		if (!memcmp(request, "Attached", 8)) {
			gdb_pack_str(pkt, "1");
		}
		break;
	case 'C':
		if (request[1] == '#') {
			gdb_pack_str(pkt, "QC00");
		}
		break;
	case 'f':
		if (!memcmp(request, "fThreadInfo", 11)) {
			gdb_pack_str(pkt, "m00");
		}
		break;
	case 's':
		if (!memcmp(request, "sThreadInfo", 11)) {
			gdb_pack_str(pkt, "l");
		}
		break;
	case 'S':
		if (!memcmp(request, "Supported", 9)) {
			gdb_pack_str(pkt, "qXfer:features:read+;"
			                  "PacketSize=" MACRO_STRING(PACKET_MAX_SIZE));
		}
		else if (!memcmp(request, "Symbol", 6)) {
			gdb_pack_str(pkt, "OK");
		}
		break;
	case 'X':
		if (!memcmp(request, "Xfer:features:read", 18)) {
			gdb_pack_str(pkt, FEATURE_STR);
		}
		break;
	default:
		break;
	}
}

static int process_packet(struct gdb_packet *pkt, void *regs) {
	char request;

	pkt->size = 1;
	pkt->checksum = 0;
	request = pkt->buf[1];

	switch (request) {
	case '?':
		gdb_pack_str(pkt, "S05");
		break;
	case 'D':
	case 'k':
		gdb_remove_all_bpts();
	case 'c':
		return 1;
	case 'g':
		gdb_pack_mem(pkt, regs, GEN_REGS_SIZE);
		break;
	case 'H':
		gdb_pack_str(pkt, "OK");
		break;
	case 'm':
		handle_read_mem(pkt);
		break;
	case 'p':
		handle_get_reg(pkt, regs);
		break;
	case 'q':
		handle_gen_query(pkt);
		break;
	case 'z':
		handle_set_remove_bpt(pkt, gdb_remove_bpt);
		break;
	case 'Z':
		handle_set_remove_bpt(pkt, gdb_set_bpt);
		break;
	default:
		break;
	}

	pkt->buf[pkt->size++] = '#';
	pkt->buf[pkt->size++] = HEXCHARS[pkt->checksum >> 4];
	pkt->buf[pkt->size++] = HEXCHARS[pkt->checksum & 0xf];

	return 0;
}

void gdb_stub(void *regs) {
	struct gdb_packet pkt;

	if (_gdb_connected) {
		pkt.buf[0] = '$';
		pkt.buf[1] = '?';
		process_packet(&pkt, regs);
		_gdb_ops.put_packet(pkt.buf, pkt.size);
	}
	else {
		gdb_remove_all_bpts();
		_gdb_connected = true;
	}

	for (;;) {
		if (_gdb_ops.get_packet(pkt.buf, sizeof(pkt.buf)) <= 0) {
			gdb_remove_all_bpts();
			break;
		}
		_gdb_ops.put_packet("+", 1);
		if (process_packet(&pkt, regs)) {
			break;
		}
		_gdb_ops.put_packet(pkt.buf, pkt.size);
	}
}
