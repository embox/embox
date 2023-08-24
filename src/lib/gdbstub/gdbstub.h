/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#ifndef SRC_LIB_GDBSTUB_H_
#define SRC_LIB_GDBSTUB_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/cdefs.h>

#include <module/embox/lib/gdbstub.h>

#define GDB_CMD_CONT   0
#define GDB_CMD_DETACH 1
#define GDB_CMD_TERM   2
#define GDB_CMD_OTHER  3

#define GDB_INTERRUPT_CHAR '\x03'

struct gdb_packet {
	char buf[GDB_PACKET_SIZE + 4];
	size_t size;
	uint8_t checksum;
};

__BEGIN_DECLS

extern int gdb_process_packet(struct gdb_packet *pkt, struct bpt_context *ctx);
extern void gdb_process_cmd(int cmd, struct gdb_packet *pkt);

extern size_t gdb_read_reg(struct bpt_context *ctx, int regnum, void *regval);
extern size_t gdb_write_reg(struct bpt_context *ctx, int regnum,
    const void *regval);

__END_DECLS

#endif /* SRC_LIB_GDBSTUB_H_ */
