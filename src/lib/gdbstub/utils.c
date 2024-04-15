/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <debug/gdbstub.h>

static const char *hexchars = "0123456789abcdef";

void gdb_pack_str(struct gdb_packet *pkt, const char *str) {
	char ch;

	while ((ch = *str++)) {
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
	}
}

void gdb_pack_mem(struct gdb_packet *pkt, const void *mem, size_t nbyte) {
	const uint8_t *ptr;
	const uint8_t *end;
	uint8_t byte;
	char ch;

	ptr = (const uint8_t *)mem;
	end = ptr + nbyte;

	while (ptr != end) {
		byte = *ptr++;
		ch = hexchars[byte >> 4];
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
		ch = hexchars[byte & 0xf];
		pkt->checksum += ch;
		pkt->buf[pkt->size++] = ch;
	}
}

void gdb_pack_begin(struct gdb_packet *pkt) {
	pkt->buf[0] = '$';
	pkt->size = 1;
	pkt->checksum = 0;
}

void gdb_pack_end(struct gdb_packet *pkt) {
	pkt->buf[pkt->size++] = '#';
	pkt->buf[pkt->size++] = hexchars[pkt->checksum >> 4];
	pkt->buf[pkt->size++] = hexchars[pkt->checksum & 0xf];
}

bool gdb_check_mem(uintptr_t addr) {
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
	int i;
	bool ret;

	ret = false;

	for (i = 0; i < 4; i++) {
		if ((sect[i].base <= addr) && (addr < sect[i].base + sect[i].len)) {
			ret = true;
			break;
		}
	}

	return ret;
}

#if 0
#include <mem/mmap.h>
#include <lib/libds/dlist.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>

bool gdb_check_mem(uintptr_t addr) {
	struct marea *marea;
	struct emmap *emmap;
	bool ret;

	ret = false;
	emmap = task_resource_mmap(task_kernel_task());

	dlist_foreach_entry(marea, &emmap->marea_list, mmap_link) {
		if (addr && (marea->start <= addr) &&
		    (addr < marea->start + marea->size)) {
			ret = true;
			break;
		}
	}

	return ret;
}
#endif
