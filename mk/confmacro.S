/*
 * EMBOX C PreProcessor-based Configuration file
 *
 * Author: Eldar Abusalimov
 */

#ifdef MAKE

#define mod(mod) MODS_ENABLE += mod

#define option_value(option, value) CONFIG_##option := value
#define option(option) option_value(option, 1)

/*#define test(test_name, level, on_boot) option(TEST_##test_name)*/
#define test(test_name, level, on_boot) \
	mod(embox.express_tests.##test_name)

#else

#define mod(mod)

#define option(option) option_value(option, 1)
#define option_value(option, value) \
	$define CONFIG_##option value

#define test(test_name, level, on_boot) \
	option(test_name) \
	$define test_##test_name##_LEVEL level \
	$define test_##test_name##_ONBOOT on_boot

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

#ifdef MAKE
#include "build.conf"
#endif /* MAKE */
#ifdef LDS
#include "lds.conf"
#endif /* LDS */

#include "platform.conf"
#include "embox.conf"
