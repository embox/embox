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
#include <sys/cdefs.h>

#include <module/embox/lib/gdbstub.h>

struct gdb_regs;

enum gdbstub_cmd {
	GDBSTUB_CONT,
	GDBSTUB_DETACH,
	GDBSTUB_SEND_REPLY
};

struct gdb_packet {
	size_t size;
	uint8_t checksum;
	char buf[PACKET_SIZE + 4];
};

struct gdb_arch {
	int (*insert_bpt)(void *addr, int type);
	int (*remove_bpt)(void *addr, int type);
	void (*remove_all_bpts)(void);
	size_t (*read_reg)(struct gdb_regs *regs, unsigned regnum, void *regval);
};

struct gdbstub_env {
	enum gdbstub_cmd cmd;
	struct gdb_arch *arch;
	struct gdb_regs *regs;
	struct gdb_packet packet;
};

__BEGIN_DECLS

extern void gdb_process_packet(struct gdbstub_env *env);

extern void gdb_arch_init(struct gdb_arch *arch);
extern void gdb_arch_prepare(void (*entry)(struct gdb_regs *));
extern void gdb_arch_cleanup(void);

__END_DECLS

#endif /* SRC_LIB_GDBSTUB_H_ */
