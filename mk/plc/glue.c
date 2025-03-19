/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.12.24
 */

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <plc/core.h>
#include <util/macro.h>

#include "iec_embox_lib.h"

#ifndef __LOCATED_VAR_LIST
#define __LOCATED_VAR_LIST
#endif

#ifndef __CONFIG_NAME
#define __CONFIG_NAME "config"
#endif

#define __LOCATED_VAR(...) \
	__LOCATED_VAR_N(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)(__VA_ARGS__)
#define __LOCATED_VAR_N(_9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) \
	MACRO_CONCAT(__LOCATED_VAR_, N)

#define __LOCATED_VAR_DEF(type, name) \
	static type __##name;             \
	type *name = &__##name;

#define __LOCATED_VAR_5(type, name, format, size, addr) \
	__LOCATED_VAR_DEF(type, name)
#define __LOCATED_VAR_7(type, name, format, size, port, pin, alt) \
	__LOCATED_VAR_DEF(type, name)
__LOCATED_VAR_LIST;
#undef __LOCATED_VAR_5
#undef __LOCATED_VAR_7

#define __LOCATED_VAR_5(type, name, format, size, addr)
#define __LOCATED_VAR_7(type, name, format, size, port, pin, alt) \
	{                                                             \
	    &__##name,                                                \
	    PLC_PHYS_VAR_##format##size,                              \
	    port,                                                     \
	    pin,                                                      \
	    alt,                                                      \
	},
static const struct plc_phys_var phys_vars[] = {
    __LOCATED_VAR_LIST /* last element -> */ {NULL},
};
#undef __LOCATED_VAR_5
#undef __LOCATED_VAR_7

#define __LOCATED_VAR_7(type, name, format, size, port, pin, alt)
#define __LOCATED_VAR_5(type, name, format, size, addr) \
	{                                                   \
	    &__##name,                                      \
	    PLC_SLAVE_VAR_##format##size,                   \
	    addr,                                           \
	},
static const struct plc_slave_var slave_vars[] = {
    __LOCATED_VAR_LIST /* last element -> */ {NULL},
};

/**
 * Functions and variables provied by generated C softPLC
 */
extern unsigned long long common_ticktime__;
extern unsigned long greatest_tick_count__;

extern void config_init__(void);
extern void config_run__(unsigned long tick);

static const struct plc_config plc_config = {
    .name = __CONFIG_NAME,
    .phys_vars = phys_vars,
    .slave_vars = slave_vars,
    .common_ticktime = &common_ticktime__,
    .greatest_tick_count = &greatest_tick_count__,
    .ops = ((struct plc_config_ops){
        .init = config_init__,
        .run = config_run__,
    }),
};

PLC_CONFIG_REGISTER(&plc_config);
