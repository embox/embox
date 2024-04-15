/**
 * @file
 * @brief
 *
 * @date 27.0.2013
 * @author Andrey Gazukin
 */

#include <stdio.h>

#include <fs/kfsop.h>

int rename(const char *src_name, const char *dst_name) {
	return krename(src_name, dst_name);
}
