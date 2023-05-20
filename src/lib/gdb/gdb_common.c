/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 15.05.23
 */
#include <stddef.h>
#include <stdint.h>

#include "gdb_priv.h"

bool _gdb_connected;
struct gdb_ops _gdb_ops;

void gdb_prepare(struct gdb_ops *ops) {
	if (_gdb_connected) {
		gdb_cleanup();
	}
	gdb_prepare_arch();
	_gdb_ops = *ops;
}

void gdb_cleanup(void) {
	gdb_cleanup_arch();
	gdb_remove_all_bpts();
	_gdb_connected = false;
}
