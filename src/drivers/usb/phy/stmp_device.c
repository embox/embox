/**
 * @file
 *
 * @date Nov 30, 2019
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <stdint.h>

#include <hal/reg.h>

#define STMP_OFFSET_REG_SET    0x4
#define STMP_OFFSET_REG_CLR    0x8
#define STMP_OFFSET_REG_TOG    0xc

#define STMP_MODULE_CLKGATE    (1 << 30)
#define STMP_MODULE_SFTRST     (1 << 31)

static inline void udelay(volatile int usec) {
	usec *= 1000;
	while (--usec);
}

#define writel(val, addr) REG32_STORE(addr, val)
/*
 * Clear the bit and poll it cleared.  This is usually called with
 * a reset address and mask being either SFTRST(bit 31) or CLKGATE
 * (bit 30).
 */
static int stmp_clear_poll_bit(void *addr, uint32_t mask)
{
	int timeout = 0x400;

	writel(mask, addr + STMP_OFFSET_REG_CLR);
	udelay(1);
	while ((REG32_LOAD(addr) & mask) && --timeout) {
		/* nothing */;

	}

	return !timeout;
}

int stmp_reset_block(void *reset_addr) {
	int ret;
	int timeout = 0x40000;

	/* clear and poll SFTRST */
	ret = stmp_clear_poll_bit(reset_addr, STMP_MODULE_SFTRST);
	if (ret)
		goto error;

	/* clear CLKGATE */
	writel(STMP_MODULE_CLKGATE, reset_addr + STMP_OFFSET_REG_CLR);

	/* set SFTRST to reset the block */
	writel(STMP_MODULE_SFTRST, reset_addr + STMP_OFFSET_REG_SET);
	udelay(1);

	/* poll CLKGATE becoming set */
	while ((!(REG32_LOAD(reset_addr) & STMP_MODULE_CLKGATE)) && --timeout) {
		/* nothing */;
	}
	if (!timeout) {
		log_error("module reset timeout (addr=%p)\n", reset_addr);
		goto error;
	}

	/* clear and poll SFTRST */
	ret = stmp_clear_poll_bit(reset_addr, STMP_MODULE_SFTRST);
	if (ret)
		goto error;

	/* clear and poll CLKGATE */
	ret = stmp_clear_poll_bit(reset_addr, STMP_MODULE_CLKGATE);
	if (ret)
		goto error;

	return 0;

error:
	log_error("module reset timeout (addr=%p)\n", reset_addr);
	return -ETIMEDOUT;
}
