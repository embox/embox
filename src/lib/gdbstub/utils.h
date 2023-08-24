/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.08.23
 */
#ifndef SRC_LIB_GDBSTUB_UTILS_H_
#define SRC_LIB_GDBSTUB_UTILS_H_

#include <stddef.h>
#include <stdint.h>

#include <debug/gdbstub.h>

extern void gdb_pack_str(struct gdb_packet *pkt, const char *str);
extern void gdb_pack_mem(struct gdb_packet *pkt, const void *mem, size_t nbyte);
extern void gdb_pack_begin(struct gdb_packet *pkt);
extern void gdb_pack_end(struct gdb_packet *pkt);
extern bool gdb_check_mem(uintptr_t addr);

#endif /* SRC_LIB_GDBSTUB_UTILS_H_ */
