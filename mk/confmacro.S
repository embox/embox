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
	$define LDS_REGION_BASE_##name base \
	$define LDS_REGION_SIZE_##name size

#define lds_section_load(name, vma_region, lma_region) \
	$define LDS_SECTION_VMA_##name vma_region \
	$define LDS_SECTION_LMA_##name lma_region

#define lds_section(name, region) \
	lds_section_load(name, region, region)

#else

#define lds_region(name, base, size)
#define lds_section(name, region)
#define lds_section_load(name, vma_region, lma_region)

#endif /* LDS */

#include "common.conf"
#include "lds.conf"
#include "drivers.conf"
#include "fs.conf"
#include "net.conf"
#include "tests.conf"
#include "usr.conf"
#include "ugly.conf"
