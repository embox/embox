/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#ifndef LIB_GDB_GDB_H_
#define LIB_GDB_GDB_H_

#include <sys/types.h>

struct gdb_ops {
	ssize_t (*get_packet)(char *dst, size_t nbyte);
	ssize_t (*put_packet)(const char *src, size_t nbyte);
};

extern void gdb_prepare(struct gdb_ops *ops);
extern void gdb_cleanup(void);

extern int gdb_set_bpt(void *addr);
extern int gdb_remove_bpt(void *addr);
extern void gdb_remove_all_bpts(void);

#endif /* LIB_GDB_GDB_H_ */
