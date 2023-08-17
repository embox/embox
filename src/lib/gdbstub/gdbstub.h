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

#define GDB_BPT_TYPE_SOFT  0
#define GDB_BPT_TYPE_HARD  1
#define GDB_BPT_TYPE_WATCH 2

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

struct gdbstub_env {
	struct gdb_regs *regs;
};

typedef void (*gdb_handler_t)(struct gdb_regs *regs);

__BEGIN_DECLS

extern int gdb_process_packet(struct gdb_packet *pkt);
extern void gdb_process_cmd(int cmd, struct gdb_packet *pkt);

extern void gdbstub_env_save(struct gdbstub_env *env);
extern void gdbstub_env_restore(struct gdbstub_env *env);

extern void gdb_set_handler(gdb_handler_t handler);

extern bool gdb_set_bpt(int type, void *addr, int kind);
extern bool gdb_remove_bpt(int type, void *addr, int kind);
extern void gdb_remove_all_bpts(void);
extern void gdb_activate_all_bpts(void);
extern void gdb_deactivate_all_bpts(void);
extern bool gdb_enable_bpts(int type);
extern void gdb_disable_bpts(int type);

extern size_t gdb_read_reg(struct gdb_regs *regs, int regnum, void *regval);
extern size_t gdb_write_reg(struct gdb_regs *regs, int regnum, void *regval);

__END_DECLS

#endif /* SRC_LIB_GDBSTUB_H_ */
