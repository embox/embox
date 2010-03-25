/*
 * EMBOX C PreProcessor-based Configuration file
 *
 * Author: Eldar Abusalimov
 */

#define __FIRST_ARG(arg, ...) arg

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
	RUNLEVEL-##mod := __FIRST_ARG(__VA_ARGS__)

#define test(mod_name, ...) \
	mod(embox.test.##mod_name, $(or __VA_ARGS__, 1))
#define test_platform(mod_name, ...) \
	mod($(if $(PLATFORM),$(PLATFORM).test.##mod_name), $(or __VA_ARGS__,1))

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

#define lds_region(name, base, size) \
	$define LDS_REGION_BASE_##name base $N\
	$define LDS_REGION_SIZE_##name size

#define lds_section_load(name, vma_region, lma_region) \
	$define LDS_SECTION_VMA_##name vma_region $N\
	$define LDS_SECTION_LMA_##name lma_region

#define lds_section(name, region) \
	lds_section_load(name, region, region)

#include "lds.conf"

#else
#error "Illegal invocation"
#endif
