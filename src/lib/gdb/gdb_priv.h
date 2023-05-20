/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#ifndef LIB_GDB_GDB_PRIV_H_
#define LIB_GDB_GDB_PRIV_H_

#include <stddef.h>
#include <stdbool.h>

#include <module/embox/arch/gdb.h>

#include "gdb.h"

#define HEXCHARS "0123456789abcdef"

struct gdb_packet {
	char buf[PACKET_MAX_SIZE + 4];
	size_t size;
	uint8_t checksum;
};

extern bool _gdb_connected;
extern struct gdb_ops _gdb_ops;

extern void gdb_pack_str(struct gdb_packet *pkt, const char *str);
extern void gdb_pack_mem(struct gdb_packet *pkt, const void *mem, size_t nbyte);

extern size_t gdb_get_reg(void *regs, unsigned int regnum, void *regval);

extern void gdb_prepare_arch(void);
extern void gdb_cleanup_arch(void);

#endif /* LIB_GDB_GDB_PRIV_H_ */
