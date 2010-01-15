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

#define lds_region(name, base, size) \
	$define LDS_REGION_##name \
	REGION_##name : ORIGIN =##base, LENGTH =##size

#define lds_section_lma_yes(name, region, lma) \
	$define PLACE_##name REGION_##region \
	$define LMA_##name AT ( ADDR(##lma) + SIZEOF (##lma) )

#define lds_section_lma_no(name, region) \
	$define PLACE_##name REGION_##region \
	$define LMA_##name

#else

#define lds_region(name, base, size)
#define lds_section(name, region, overloaded, lma)
#define lds_section_lma_no(name, region)
#define lds_section_lma_yes(name, region, lma)

#endif /* LDS */

#include "common.conf"
#include "lds.conf"
#include "drivers.conf"
#include "fs.conf"
#include "net.conf"
#include "tests.conf"
#include "usr.conf"
#include "ugly.conf"
