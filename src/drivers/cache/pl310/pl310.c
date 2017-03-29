/**
 * @file
 *
 * @date 28.03.2017
 * @author Anton Bondarev
 */
#include <errno.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/reg.h>

#include <util/log.h>

#include "pl310.h"

#include <embox/unit.h>
#include <framework/mod/options.h>

#define PL310_BASE    OPTION_GET(NUMBER,base_addr)

EMBOX_UNIT_INIT(pl310_init);

#define CACHE_LINE_SIZE     32

static int pl310_init(void) {
	log_debug("\n0x%X 0x%X", L2X0_CACHE_ID, REG_LOAD(PL310_BASE + L2X0_CACHE_ID));
	log_debug("0x%X 0x%X", L2X0_CACHE_TYPE, REG_LOAD(PL310_BASE + L2X0_CACHE_TYPE));
	log_debug("0x%X 0x%X", L2X0_CTRL, REG_LOAD(PL310_BASE + L2X0_CTRL));
	log_debug("0x%X 0x%X", L2X0_AUX_CTRL, REG_LOAD(PL310_BASE + L2X0_AUX_CTRL));
	return 0;
}

static struct periph_memory_desc pl310_mem = {
	.start = PL310_BASE,
	.len   = 0x1000
};

PERIPH_MEMORY_DEFINE(pl310_mem);
