/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.09.23
 */
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <drivers/common/memory.h>
#include <framework/mod/options.h>

#include "vic.h"

static_assert(VIC_DEVICE_COUNT > 0, "VIC_DEVICE_COUNT must be "
                                    "greater than zero.");

#define VIC0 OPTION_GET(NUMBER, vic0)
#define VIC1 OPTION_GET(NUMBER, vic1)

#if VIC_DEVICE_COUNT == 1
const uintptr_t vic_devices[] = {VIC0};

PERIPH_MEMORY_DEFINE(vic0, VIC0, 0x1000);
#elif VIC_DEVICE_COUNT == 2
const uintptr_t vic_devices[] = {VIC0, VIC1};

PERIPH_MEMORY_DEFINE(vic0, VIC0, 0x1000);
PERIPH_MEMORY_DEFINE(vic1, VIC1, 0x1000);
#else
#error "Too many vic devices!"
/* TODO: support more vic devices */
#endif
