/*
 * EMBOX C PreProcessor-based Configuration file
 *
 * Author: Eldar Abusalimov
 */
#ifdef MAKE

#define target(target) TARGET:=target
#define arch(arch) ARCH:=arch
#define make(str) str
#define option(option) MONITOR_##option=y

#else

#define target(target)
#define arch(arch)
#define make(str)
#define option(option) \
	$define CONFIG_##option

#endif /* MAKE */

#ifdef LDS
#define lds_ram(base, size) \
	$define LDS_RAM_BASE base \
	$define LDS_RAM_SIZE size
#define lds_rom(base, size) \
	$define LDS_ROM_BASE base \
	$define LDS_ROM_SIZE size
#else
#define lds_ram(base, size)
#define lds_rom(base, size)
#endif /* LDS */

#include "common.conf"
#include "lds.conf"
#include "drivers.conf"
#include "fs.conf"
#include "net.conf"
#include "tests.conf"
#include "usr.conf"
#include "ugly.conf"
