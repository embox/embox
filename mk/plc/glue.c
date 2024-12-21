/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.12.24
 */

#include <stddef.h>

#include <plc/core.h>

#include "iec_embox_lib.h"

#define __LOCATED_VAR(type, name, format, size, ...) \
	static type __##name;                            \
	static type *name = &__##name;
__LOCATED_VAR_LIST;
#undef __LOCATED_VAR
#define __LOCATED_VAR(type, name, format, size, ...) \
	{&__##name, PLC_LOCATED_##format##size, __VA_ARGS__},
struct plc_located_var __plc_located_var_list[] = {__LOCATED_VAR_LIST{NULL}};
#undef __LOCATED_VAR
