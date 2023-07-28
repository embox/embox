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
#include <sys/mman.h>

#include <util/macro.h>
#include <debug/gdbstub.h>
#include <framework/mod/options.h>

#define HEXCHARS "0123456789abcdef"

#define USE_VMEM OPTION_GET(BOOLEAN, vmem)

static void gdb_pack_str(struct gdb_packet *pkt, const char *str) {
	char ch;

	while ((ch = *str++)) {
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
	}
}

static void gdb_pack_mem(struct gdb_packet *pkt, const void *mem,
    size_t nbyte) {
	const uint8_t *ptr;
	const uint8_t *end;
	uint8_t byte;
	char ch;

	ptr = (const uint8_t *)mem;
	end = ptr + nbyte;

	while (ptr != end) {
		byte = *ptr++;
		ch = HEXCHARS[byte >> 4];
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
		ch = HEXCHARS[byte & 0xf];
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
	}
}

static void gdb_read_regs_cmd(struct gdbstub_env *env) {
	unsigned long regval;
	unsigned regnum;
	size_t regsize;

	for (regnum = 0;; regnum++) {
		regsize = env->arch->read_reg(env->regs, regnum, &regval);
		if (!regsize) {
			break;
		}
		gdb_pack_mem(&env->packet, &regval, regsize);
	}
}

static void gdb_read_reg_cmd(struct gdbstub_env *env) {
	unsigned long regval;
	unsigned regnum;
	size_t regsize;

	regnum = strtoul(env->packet.buf + 2, NULL, 16);
	regsize = env->arch->read_reg(env->regs, regnum, &regval);
	if (regsize) {
		gdb_pack_mem(&env->packet, &regval, regsize);
	}
	else {
		gdb_pack_str(&env->packet, "E01");
	}
}

#if USE_VMEM
#include <sys/mman.h>

#include <mem/mmap.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <util/dlist.h>

static void gdb_read_mem_cmd(struct gdbstub_env *env) {
	struct marea *marea;
	struct emmap *emmap;
	char *endptr;
	uintptr_t addr;
	size_t len;

	emmap = task_resource_mmap(task_kernel_task());
	addr = strtoul(env->packet.buf + 2, &endptr, 16);

	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		if (addr && (marea->start <= addr) &&
		    (addr < marea->start + marea->size) &&
		    (marea->flags & PROT_WRITE)) {
			len = strtoul(endptr + 1, NULL, 16);
			gdb_pack_mem(&env->packet, (void *)addr, len);
			return;
		}
	}
	gdb_pack_str(&env->packet, "E01");
}
#else
static void gdb_read_mem_cmd(struct gdbstub_env *env) {
	extern char _text_vma, _rodata_vma, _data_vma, _bss_vma;
	extern char _text_len, _rodata_len, _data_len, _bss_len_with_reserve;

	struct {
		uintptr_t base;
		uintptr_t len;
	} sect[4] = {
	    {(uintptr_t)&_bss_vma, (uintptr_t)&_bss_len_with_reserve},
	    {(uintptr_t)&_text_vma, (uintptr_t)&_text_len},
	    {(uintptr_t)&_data_vma, (uintptr_t)&_data_len},
	    {(uintptr_t)&_rodata_vma, (uintptr_t)&_rodata_len},
	};

	char *endptr;
	uintptr_t addr;
	size_t len;
	int i;

	addr = strtoul(env->packet.buf + 2, &endptr, 16);
	if (addr) {
		for (i = 0; i < 4; i++) {
			if ((sect[i].base <= addr) && (addr < sect[i].base + sect[i].len)) {
				len = strtoul(endptr + 1, NULL, 16);
				gdb_pack_mem(&env->packet, (void *)addr, len);
				return;
			}
		}
	}

	gdb_pack_str(&env->packet, "E01");
}
#endif

static void gdb_bpt_cmd(struct gdbstub_env *env) {
	bool (*fn)(void *addr, int type);
	char *endptr;
	void *addr;
	int type;

	if (env->packet.buf[1] == 'Z') {
		fn = env->arch->insert_bpt;
	}
	else {
		fn = env->arch->remove_bpt;
	}

	addr = (void *)strtoul(env->packet.buf + 4, &endptr, 16);
	type = strtoul(endptr + 1, NULL, 16);

	if (fn(addr, type)) {
		gdb_pack_str(&env->packet, "OK");
	}
	else {
		gdb_pack_str(&env->packet, "E01");
	}
}

static void gdb_query_cmd(struct gdbstub_env *env) {
	char *request;

	request = env->packet.buf + 2;

	switch (*request) {
	case 'A':
		if (!memcmp(request, "Attached", 8)) {
			gdb_pack_str(&env->packet, "1");
		}
		break;
	case 'C':
		if (request[1] == '#') {
			gdb_pack_str(&env->packet, "QC00");
		}
		break;
	case 'f':
		if (!memcmp(request, "fThreadInfo", 11)) {
			gdb_pack_str(&env->packet, "m00");
		}
		break;
	case 's':
		if (!memcmp(request, "sThreadInfo", 11)) {
			gdb_pack_str(&env->packet, "l");
		}
		break;
	case 'S':
		if (!memcmp(request, "Supported", 9)) {
			gdb_pack_str(&env->packet, "qXfer:features:read+;PacketSize=");
			sprintf(env->packet.buf + env->packet.size, "%lx", GDB_PACKET_SIZE);
			gdb_pack_str(&env->packet, env->packet.buf + env->packet.size);
		}
		else if (!memcmp(request, "Symbol", 6)) {
			gdb_pack_str(&env->packet, "OK");
		}
		break;
	case 'X':
		if (!memcmp(request, "Xfer:features:read", 18)) {
			gdb_pack_str(&env->packet, GDB_FEATURE_STR);
		}
		break;
	default:
		break;
	}
}

void gdb_process_packet(struct gdbstub_env *env) {
	struct gdb_packet *pkt;

	pkt = &env->packet;

	pkt->buf[0] = '$';
	pkt->size = 1;
	pkt->checksum = 0;

	env->cmd = GDBSTUB_SEND_REPLY;

	switch (pkt->buf[1]) {
	case '?': /* get status */
		gdb_pack_str(pkt, "S05");
		break;
	case 'c': /* continue */
	case 's': /* step */
		gdb_pack_str(pkt, "S05");
		env->cmd = GDBSTUB_CONT;
		break;
	case 'D': /* detach */
	case 'k': /* kill */
		env->cmd = GDBSTUB_DETACH;
		break;
	case 'g': /* read general registers */
		gdb_read_regs_cmd(env);
		break;
	case 'H': /* set thread */
		gdb_pack_str(pkt, "OK");
		break;
	case 'm': /* read memory */
		gdb_read_mem_cmd(env);
		break;
	case 'p': /* read register */
		gdb_read_reg_cmd(env);
		break;
	case 'q': /* query command */
		gdb_query_cmd(env);
		break;
	case 'Z': /* insert breakpoint */
	case 'z': /* remove breakpoint */
		gdb_bpt_cmd(env);
		break;
	default:
		break;
	}

	pkt->buf[pkt->size++] = '#';
	pkt->buf[pkt->size++] = HEXCHARS[pkt->checksum >> 4];
	pkt->buf[pkt->size++] = HEXCHARS[pkt->checksum & 0xf];
}
