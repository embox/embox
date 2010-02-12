/**
 * @file autoconf.h
 */

#define CONFIG(option) (defined(CONFIG_##option))

#include "asm/cpu_conf.h"
