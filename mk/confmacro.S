/*
 * Embox C PreProcessor-based Configuration file
 *
 * Author: Eldar Abusalimov
 */

#define __FIRST_ARG(arg, ...) arg

#define define(macro, value) \
	$define macro value

#if defined(__BUILD_MK__)

#include "build.conf"

/* Note: $(or __VA_ARGS__) used to extract first arg only (if any). */
#define option(option, ...) \
	CONFIG_##option := $(or __VA_ARGS__,1)

#include "options.conf"

#elif defined(__MODS_MK__)

#define option(option, ...) \
	$(warning Options defined outside "options.conf" are ignored)

#define mod(mod, ...) \
	MODS_ENABLE += mod $N\
	RUNLEVEL-##mod := $(or __FIRST_ARG(__VA_ARGS__), 2)

#define test(mod_name, ...) \
	mod(embox.test.##mod_name, $(or __VA_ARGS__, 1))
#define test_platform(mod_name, ...) \
	mod($(if $(PLATFORM),$(PLATFORM).test.##mod_name), $(or __VA_ARGS__,1))

#define example(mod_name, ...) \
	mod(embox.example.##mod_name, $(or __VA_ARGS__, 1))

#define cmd(mod_name) \
	mod(embox.cmd.##mod_name)
#define cmd_platform(mod_name) \
	mod($(if $(PLATFORM),$(PLATFORM).cmd.##mod_name))

#include "mods.conf"

#elif defined(__CONFIG_H__)

#define option(option, ...) \
	$define CONFIG_##option __FIRST_ARG(__VA_ARGS__)

#include "options.conf"

#elif defined(__CONFIG_LDS_H__)

#define region(name, base, size) \
	$N$define LDS_REGION_BASE_##name base \
	$N$define LDS_REGION_SIZE_##name size

#define section(name, region, ...) \
	__section(name, region, __VA_ARGS__)

#define __defined(macro) \
	defined(macro)

/* This indirection is for var-arg handling. */
#define __section(name, region, lma_region) \
	$N$define LDS_SECTION_VMA_##name region         \
	$N$if __defined(lma_region##LDS_SECTION_VMA_##name) \
	$N$ define LDS_SECTION_LMA_##name region        \
	$N$else                                         \
	$N$ define LDS_SECTION_LMA_##name lma_region    \
	$N$endif

/* Backward compatibility. */

#define lds_region(name, base, size) \
	$N$warning "lds_region() is deprecated, use region() instead" \
	$Nregion(name, region)
#define lds_section(name, region) \
	$N$warning "lds_section() is deprecated, use section() instead" \
	$Nsection(name, region)
#define lds_section_load(name, vma_region, lma_region) \
	$N$warning "lds_section_load() is deprecated, use section() instead" \
	$Nsection(name, vma_region, lma_region)

/* Predefined regions/sections. */
#define text(region, ...)    __section(text,   region, __VA_ARGS__)
#define rodata(region, ...)  __section(rodata, region, __VA_ARGS__)
#define data(region, ...)    __section(data,   region, __VA_ARGS__)
#define bss(region, ...)     __section(bss,    region, __VA_ARGS__)

#define RAM(origin, length)  region(RAM, origin, length)
#define ROM(origin, length)  region(ROM, origin, length)

/* Region alias. */
#define SDRAM RAM
#define FLASH ROM

#include "lds.conf"

#else
#error "Illegal invocation"
#endif
