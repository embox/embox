/**
 * @file
 * @brief OMAP3 General Purpose Memory Controller
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <kernel/printk.h>
#include <embox/unit.h>
#include <hal/reg.h>

#define GPMC_MEMORY_START  0x00000000
#define GPMC_MEMORY_END    0x3fffffff
#define GPMC_CS_MEMORY_MAX 0x01000000

#define GPMC_BASE_ADDRESS 0x6E000000

/* GPMC Registers Mapping Summary (address offsets) */
#define GPMC_REVISION           0x00
#define GPMC_SYSCONFIG          0x10
#define GPMC_SYSSTATUS          0x14
#define GPMC_IRQSTATUS          0x18
#define GPMC_IRQENABLE          0x1c
#define GPMC_TIMEOUT_CONTROL    0x40
#define GPMC_ERR_ADDRESS        0x44
#define GPMC_ERR_TYPE           0x48
#define GPMC_CONFIG             0x50
#define GPMC_STATUS             0x54
#define GPMC_PREFETCH_CONFIG1   0x1e0
#define GPMC_PREFETCH_CONFIG2   0x1e4
#define GPMC_PREFETCH_CONTROL   0x1ec
#define GPMC_PREFETCH_STATUS    0x1f0
#define GPMC_ECC_CONFIG         0x1f4
#define GPMC_ECC_CONTROL        0x1f8
#define GPMC_ECC_SIZE_CONFIG    0x1fC
#define GPMC_BCH_SWDATA         0x000002d0

#define GPMC_CS_CONFIG1         0x60
#define GPMC_CS_CONFIG2         0x64
#define GPMC_CS_CONFIG3         0x68
#define GPMC_CS_CONFIG4         0x6c
#define GPMC_CS_CONFIG5         0x70
#define GPMC_CS_CONFIG6         0x74
#define GPMC_CS_CONFIG7         0x78

#define GPMC_CS0_OFFSET         0x60
#define GPMC_CS_SIZE            0x30

#define GPMC_CHUNK_SHIFT        24 /* 16 MB */
#define GPMC_SECTION_SHIFT      28 /* 128 MB */

#define GPMC_REVISION_MAJOR(l) ((l >> 4) & 0xF)
#define GPMC_REVISION_MINOR(l) (l & 0xF)

/* Register's fields masks */
#define GPMC_CONFIG7_CSVALID    (1 << 6)

#define GPMC_CS_NUM 8

EMBOX_UNIT_INIT(gpmc_init);

static uint32_t gpmc_reg_read(int offset) {
	return REG_LOAD(GPMC_BASE_ADDRESS + offset);
}

//static void gpmc_reg_write(int offset, uint32_t val) {
//	REG_STORE(base_addr + offset, val);
//}

static uint32_t gpmc_cs_reg_read(int cs, int offset) {
	unsigned long reg_addr;
	reg_addr = GPMC_BASE_ADDRESS + offset + (cs * GPMC_CS_SIZE);
	return REG_LOAD(reg_addr);
}

static void gpmc_cs_reg_write(int cs, int offset, uint32_t val) {
	unsigned long reg_addr;
	reg_addr = GPMC_BASE_ADDRESS + offset + (cs * GPMC_CS_SIZE);
	REG_STORE(reg_addr, val);
}
#if 0
static int gpmc_cs_enabled(int cs) {
	uint32_t l;
	l = gpmc_cs_reg_read(cs, GPMC_CS_CONFIG7);
	return l & GPMC_CONFIG7_CSVALID;
}
#endif

static int gpmc_cs_enable_mem(int cs, uint32_t base, uint32_t size) {
	uint32_t l;
	uint32_t mask;

#if 0
	if (gpmc_cs_enabled(cs)) {
		return -1; // it is so?
	}

	/*
	* Ensure that base address is aligned on a
	* boundary equal to or greater than size.
	*/
	if (base & (size - 1))
		return -EINVAL;
#endif

	mask = (1 << GPMC_SECTION_SHIFT) - size;
	l = gpmc_cs_reg_read(cs, GPMC_CS_CONFIG7);
	l &= ~0x3f;
	l = (base >> GPMC_CHUNK_SHIFT) & 0x3f;
	l &= ~(0x0f << 8);
	l |= ((mask >> GPMC_CHUNK_SHIFT) & 0x0f) << 8;
	l |= GPMC_CONFIG7_CSVALID;
	gpmc_cs_reg_write(cs, GPMC_CS_CONFIG7, l);

	return 0;
}

int gpmc_cs_init(int cs, uint32_t *base, uint32_t size) {
	*base = GPMC_MEMORY_START + GPMC_CS_MEMORY_MAX * cs;
	return gpmc_cs_enable_mem(cs, *base, size);
}

static int gpmc_init(void) {
	uint32_t l;

	l = gpmc_reg_read(GPMC_REVISION);
	if (GPMC_REVISION_MAJOR(l) != 5 || GPMC_REVISION_MINOR(l) != 0) // revision 5.0 for OMAP35xx
		return -1;

	return 0;
}
