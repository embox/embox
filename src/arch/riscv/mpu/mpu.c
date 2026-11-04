/**
 * @file
 *
 * @date 04.11.2026
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>

#include <embox/unit.h>

#include "plf_l1cache.h"

#include <framework/mod/options.h>

#define ICACHE_EN   OPTION_GET(BOOLEAN, icache_en)
#define DCACHE_EN   OPTION_GET(BOOLEAN, dcache_en)

static int mpu_init(void) {
	plf_mpu_init(ICACHE_EN, DCACHE_EN);

#if ICACHE_EN && DCACHE_EN
	enable_l1_caches();
#elif ICACHE_EN
	enable_l1i_cache();
#elif DCACHE_EN
	enable_l1d_cache();
#endif

#if 0 // just for test
	for (int i = 0 ; i < 1000000; i++) {
	}
#endif

	return 0;
}

EMBOX_UNIT_INIT(mpu_init);
