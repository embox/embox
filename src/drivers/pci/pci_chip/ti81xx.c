/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.02.2014
 */

#include <stdint.h>
#include <hal/reg.h>

#include <hal/mmu.h>
#include <drivers/common/memory.h>
#include <mem/vmem.h>

#include <drivers/pci/pci_chip/pci_utils.h>

#include <embox/unit.h>
EMBOX_UNIT_INIT(ti81xx_pci_init);

#define TI81_CM 0x48140000
#define TI81_PCI_CFG (TI81_CM + 0x640)
#define TI81_PCI_CFG_DEVTYPE                  0x00000003
#define TI81_PCI_CFG_DEVTYPE_ROOTC            0x00000002
#define TI81_PCI_CFG_CFGPLL                   0xffff0000
#define TI81_PCI_CFG_CFGPLL_MUL25X            0x01c90000
#define TI81_PCI_CFG_STSPLL                   0x00000f00
#define TI81_PCI_CFG_STSPLL_LCK               0x00000100

#define TI81_PRCM 0x48180000
#define TI81_RM_DEF_RSTCTRL (TI81_PRCM + 0xb10)
#define TI81_RM_DEF_RSTCTRL_PCI (1 << 7)

#define TI81_RM_DEF_RSTST (TI81_PRCM + 0xb14)
#define TI81_RM_DEF_RSTST_PCI (1 << 7)

#define TI81_CM_DEF_CLKSTCTRL (TI81_PRCM + 0x510)
#define TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL       0x00000003
#define TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_SLEEP 0x00000001
#define TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_WKUP  0x00000002
#define TI81_CM_DEF_CLKSTCTRL_CLK_GCLK        0x00000100
#define TI81_CM_DEF_CLKSTCTRL_CLK_GCLK_GATED  0x00000000
#define TI81_CM_DEF_CLKSTCTRL_CLK_GCLK_ACTIV  0x00000100

#define TI81_CM_DEF_PCI_CLKCTRL (TI81_PRCM + 0x578)
#define TI81_CM_DEF_PCI_CLKCTRL_MODLMODE      0x00000003
#define TI81_CM_DEF_PCI_CLKCTRL_MODLMODE_DIS  0x00000000
#define TI81_CM_DEF_PCI_CLKCTRL_MODLMODE_EN   0x00000002
#define TI81_CM_DEF_PCI_CLKCTRL_IDLE          0x00030000
#define TI81_CM_DEF_PCI_CLKCTRL_IDLE_FUNC     0x00000000
#define TI81_CM_DEF_PCI_CLKCTRL_STANBY        0x00040000

#define TI81_PCI_REGION0 0x51000000
#define TI81_PCI_CMD_STATUS		      (TI81_PCI_REGION0 + 0x4)
#define TI81_PCI_CMD_STATUS_LTSSM_EN 	      0x00000001
#define TI81_PCI_CMD_OB_XLT_EN   	      0x00000002
#define TI81_PCI_CFG_SETUP                    (TI81_PCI_REGION0 + 0x8)
#define TI81_PCI_CFG_SETUP_TYPE               0x01000000
#define TI81_PCI_CFG_SETUP_BUS                0x00ff0000
#define TI81_PCI_CFG_SETUP_BUS_SHIFT          16
#define TI81_PCI_CFG_SETUP_DEV                0x00001f00
#define TI81_PCI_CFG_SETUP_DEV_SHIFT          8
#define TI81_PCI_CFG_SETUP_FNC                0x00000007
#define TI81_PCI_CFG_SETUP_FNC_SHIFT          0
#define TI81_PCI_OB_SIZE                      (TI81_PCI_REGION0 + 0x30)

#define TI81_PCI_OB_OFF_LO_INDEX(n)           (TI81_PCI_REGION0 + 0x200 + 8 * (n))
#define TI81_PCI_OB_OFF_HI(n)                 (TI81_PCI_REGION0 + 0x204 + 8 * (n))
#define TI81_PCI_OB_EN                        0x00000001

#define TI81_PCI_REGION0_LOCAL_CFG  (TI81_PCI_REGION0 + 0x1000)
#define TI81_PCI_VEN_DEV_ID		      (TI81_PCI_REGION0_LOCAL_CFG + 0)
#define TI81_PCI_VEN_DEV_ID_VEN_SHIFT         0
#define TI81_PCI_VEN_DEV_ID_DEV_SHIFT         16
#define TI81_PCI_STATUS_COMMAND		      (TI81_PCI_REGION0_LOCAL_CFG + 4)
#define TI81_PCI_STATUS_COMMAND_BUSMASTER     0x00000004
#define TI81_PCI_CLASSCODE		      (TI81_PCI_REGION0_LOCAL_CFG + 8)
#define TI81_PCI_CLASSCODE_SHIFT              8
#define TI81_PCI_BAR0			      (TI81_PCI_REGION0_LOCAL_CFG + 0x10)
#define TI81_PCI_BAR1			      (TI81_PCI_REGION0_LOCAL_CFG + 0x10)

#define TI81_PCI_DEBUG0	                      (TI81_PCI_REGION0_LOCAL_CFG + 0x728)
#define TI81_PCI_DEBUG0_LTSSM_MASK	      0x0000001f
#define TI81_PCI_DEBUG0_LTSSM_L0              0x00000011

#define TI81_PCI_REGION0_REMOTE_CFG (TI81_PCI_REGION0 + 0x2000)

#define TI81_PCI_REGION1 0x20000000

#define TI81_PCI_OB_WINDOW_N 32
#define TI81_PCI_OB_WINDOW_MAXSZ 3 /* in power of 1MB */

#include <kernel/printk.h>

int pci_check_type(void) {
#if 0
	int pci_type = 0;
	uint32_t tmp;

	out8(PCI_CONFIG_ADDRESS + 0x3, 0);
	out8(PCI_CONFIG_ADDRESS, 0);
	out8(PCI_CONFIG_ADDRESS + 0x2, 0);
	if (in8(PCI_CONFIG_ADDRESS) == 0 && in8(PCI_CONFIG_ADDRESS + 0x2) == 0) {
		pci_type = 2;
	} else {
		out8(PCI_CONFIG_ADDRESS + 0x3, 1);
		tmp = in32(PCI_CONFIG_ADDRESS);
		out32(PCI_CONFIG_ADDRESS, 0x80000000);
		if (in32(PCI_CONFIG_ADDRESS) == 0x80000000) {
			pci_type = 1;
		}
		out32(PCI_CONFIG_ADDRESS, tmp);
	}
	return pci_type;
#endif
	return 0;
}

static inline uint32_t ti81_pci_dev_check(uint32_t bus, uint32_t dev) {

	if (bus <= 1) {
		return dev == 0 ? PCIUTILS_SUCCESS : PCIUTILS_TIMEOUT;
	}

	if ((REG_LOAD(TI81_PCI_DEBUG0) & TI81_PCI_DEBUG0_LTSSM_MASK) !=
			TI81_PCI_DEBUG0_LTSSM_L0) {
		return PCIUTILS_INVALID;

	}

	return PCIUTILS_SUCCESS;
}

static void *ti81xx_config_base(uint32_t bus, uint32_t dev, uint32_t fn) {
	uint32_t reg;

	if (bus == 0) {
		return (void *) TI81_PCI_REGION0_LOCAL_CFG;
	}

	reg = ((bus << TI81_PCI_CFG_SETUP_BUS_SHIFT) & TI81_PCI_CFG_SETUP_BUS) |
		((dev << TI81_PCI_CFG_SETUP_DEV_SHIFT) & TI81_PCI_CFG_SETUP_DEV) |
		((fn << TI81_PCI_CFG_SETUP_FNC_SHIFT) & TI81_PCI_CFG_SETUP_FNC);

	if (bus != 1) {
		reg |= TI81_PCI_CFG_SETUP_TYPE;
	}

	REG_STORE(TI81_PCI_CFG_SETUP, reg);
	/*printk("%s: bus=%d dev=%d fn=%d %08lx\n", __func__, bus, dev, fn, REG_LOAD(TI81_PCI_CFG_SETUP));*/

	return (void *) TI81_PCI_REGION0_REMOTE_CFG;
}

static inline uint32_t ti81_pci_config_read(uint32_t bus, uint32_t dev_fn,
				uint32_t where, int size, void *ptr) {
	uint32_t ret, tmp;
	void *config;

	if (PCIUTILS_SUCCESS == (ret = ti81_pci_dev_check(bus, dev_fn >> 3))) {
		config = ti81xx_config_base(bus, dev_fn >> 3, dev_fn & 0x7);
		tmp = REG_LOAD(config + (where & ~3)) >> (8 * (where & 3));
	} else {
		tmp = ~0;
	}

	if (size == 1) {
		* (uint8_t *) ptr = tmp;
	} else if (size == 2) {
		* (uint16_t *) ptr = tmp;
	} else {
		* (uint32_t *) ptr = tmp;
	}

	return ret;;
}

static inline uint32_t ti81_pci_config_write(uint32_t bus, uint32_t dev_fn,
				uint32_t where, int size, uint32_t value) {
	uint32_t ret;
	void *config, *ptr;

	if (PCIUTILS_SUCCESS != (ret = ti81_pci_dev_check(bus, dev_fn >> 3))) {
		return ret;
	}

	config = ti81xx_config_base(bus, dev_fn >> 3, dev_fn & 0x7);

	ptr = (void *) (config + where);
	if (size == 1) {
		* (volatile uint8_t *) ptr = value;
	} else if (size == 2) {
		* (volatile uint16_t *) ptr = value;
	} else {
		* (volatile uint32_t *) ptr = value;
	}

	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value) {
	return ti81_pci_config_read(bus, dev_fn, where, 1, value);
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value) {
	return ti81_pci_config_read(bus, dev_fn, where, 2, value);
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value) {
	return ti81_pci_config_read(bus, dev_fn, where, 4, value);
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value) {
	return ti81_pci_config_write(bus, dev_fn, where, 1, value);
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value) {
	return ti81_pci_config_write(bus, dev_fn, where, 2, value);
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
		uint32_t where,	uint32_t value) {
	return ti81_pci_config_write(bus, dev_fn, where, 4, value);
}

static void ti81xx_pci_clk_enable(void) {
	unsigned long reg;

	/* do domain transition wakeup */
	REG_STORE(TI81_CM_DEF_CLKSTCTRL, TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_WKUP);
	/* do clock enable */
	REG_STORE(TI81_CM_DEF_PCI_CLKCTRL, TI81_CM_DEF_PCI_CLKCTRL_MODLMODE_EN);
	/* bring out of reset */
	REG_ANDIN(TI81_RM_DEF_RSTCTRL, ~TI81_RM_DEF_RSTCTRL_PCI);

	/* wait for reset complete */
	while (!(REG_LOAD(TI81_RM_DEF_RSTST) & TI81_RM_DEF_RSTST_PCI)) {

	}

	/* clear complete reset flag */
	REG_STORE(TI81_RM_DEF_RSTST, TI81_RM_DEF_RSTST_PCI);

	/* wait for 'idle' state of peripherial */
	while (((reg = REG_LOAD(TI81_CM_DEF_PCI_CLKCTRL)) & TI81_CM_DEF_PCI_CLKCTRL_IDLE)
		       == TI81_CM_DEF_PCI_CLKCTRL_IDLE) {
	}
}

static void ti81_pci_enable_outbound(void) {
	int i;
	unsigned long map_ptr;

	map_ptr = TI81_PCI_REGION1;

	REG_STORE(TI81_PCI_OB_SIZE, TI81_PCI_OB_WINDOW_MAXSZ);

	/* mapping 32 windows by 8MB; mapping whole 256MB REGION1 */
	for (i = 0; i < TI81_PCI_OB_WINDOW_N; i ++) {
		REG_STORE(TI81_PCI_OB_OFF_LO_INDEX(i), map_ptr | TI81_PCI_OB_EN);
		REG_STORE(TI81_PCI_OB_OFF_HI(i), 0);

		map_ptr += 1 << (TI81_PCI_OB_WINDOW_MAXSZ + 20); /* 20 is a log_2 of 1MB */
	}

	REG_ORIN(TI81_PCI_CMD_STATUS, TI81_PCI_CMD_OB_XLT_EN);
}

static int ti81xx_pci_init(void) {

	REG_STORE(TI81_PCI_CFG, TI81_PCI_CFG_DEVTYPE_ROOTC);

	ti81xx_pci_clk_enable();

	REG_ORIN(TI81_PCI_CFG, TI81_PCI_CFG_CFGPLL_MUL25X);

	while (!(REG_LOAD(TI81_PCI_CFG) & TI81_PCI_CFG_STSPLL_LCK)) {

	}
	REG_STORE(TI81_CM_DEF_CLKSTCTRL, TI81_CM_DEF_CLKSTCTRL_CLKTRCTRL_WKUP);

	REG_ANDIN(TI81_PCI_CMD_STATUS, ~TI81_PCI_CMD_STATUS_LTSSM_EN);

	/* Configure core registers ? */
	REG_ORIN(TI81_PCI_CMD_STATUS, TI81_PCI_CMD_STATUS_LTSSM_EN);
	/*printk("%s: cmd_status=%08lx\n", __func__, REG_LOAD(TI81_PCI_CMD_STATUS));*/

	REG_ORIN(TI81_PCI_CLASSCODE, 0x0604 << TI81_PCI_CLASSCODE_SHIFT);
	REG_ORIN(TI81_PCI_STATUS_COMMAND, TI81_PCI_STATUS_COMMAND_BUSMASTER);

	REG_STORE(TI81_PCI_BAR0, 0);
	REG_STORE(TI81_PCI_BAR1, 0);

	ti81_pci_enable_outbound();

	return 0;
}

/* NOTE: this may be inaccurate! */
unsigned int pci_irq_number(struct pci_slot_dev *dev) {
	return (unsigned int) dev->irq_line;
}

PERIPH_MEMORY_DEFINE(pci_chip, TI81_CM, 0x1000);

PERIPH_MEMORY_DEFINE(pci_root, TI81_PRCM, 0x1000);

PERIPH_MEMORY_DEFINE(pci_region0, TI81_PCI_REGION0, 0x4000);

PERIPH_MEMORY_DEFINE(pci_region1, TI81_PCI_REGION1, 0x100000);
