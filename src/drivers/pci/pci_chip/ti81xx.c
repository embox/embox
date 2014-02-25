/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.02.2014
 */

#include <stdint.h>
#include <hal/reg.h>

#include <drivers/pci/pci_utils.h>

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
#define TI81_PCI_CFG_SETUP                    (TI81_PCI_REGION0 + 0x8)

#define TI81_PCI_REGION0_REMOTE_CFG (TI81_PCI_REGION0 + 0x2000)

#define TI81_PCI_REGION1 0x20000000

#include <kernel/printk.h>
#define PCI_CONFIG_CMD(bus, dev_fn, where) \
		(0x80000000 | (bus << 16) | (dev_fn << 8) | (where & ~3))

#define PCI_TYPE0 0
#define PCI_TYPE1 1

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

static void ti81xx_cfg_setup(int pci_type, uint32_t bus, uint32_t dev, uint32_t fn) {

}

static inline uint32_t ti81_pci_config_read(uint32_t bus, uint32_t dev_fn,
				uint32_t where, int size) {
	uint32_t reg;

	ti81xx_cfg_setup(PCI_TYPE0, bus, dev_fn >> 3, dev_fn & 0x7);

	reg = REG_LOAD(TI81_PCI_REGION0_REMOTE_CFG + (where & ~3));

	if (size == 1) {
		return (reg >> (8 * (where & 3))) & 0xff;
	} else if (size == 2) {
		return (reg >> (8 * (where & 3))) & 0xffff;
	}

	return reg;
}

static inline void ti81_pci_config_write(uint32_t bus, uint32_t dev_fn,
				uint32_t where, int size, uint32_t value) {
	void *ptr;

	ti81xx_cfg_setup(PCI_TYPE0, bus, dev_fn >> 3, dev_fn & 0x7);

	ptr = (void *) (TI81_PCI_REGION0_REMOTE_CFG + where);
	if (size == 1) {
		* (volatile uint8_t *) ptr = value;
	} else if (size == 2) {
		* (volatile uint16_t *) ptr = value;
	} else {
		* (volatile uint32_t *) ptr = value;
	}
}

uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t *value) {
	*value = ti81_pci_config_read(bus, dev_fn, where, 1);
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t *value) {
	*value = ti81_pci_config_read(bus, dev_fn, where, 2);
	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint32_t *value) {
	*value = ti81_pci_config_read(bus, dev_fn, where, 4);
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint8_t value) {
	ti81_pci_config_write(bus, dev_fn, where, 1, value);
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn,
				uint32_t where, uint16_t value) {
	ti81_pci_config_write(bus, dev_fn, where, 2, value);
	return PCIUTILS_SUCCESS;
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn,
		uint32_t where,	uint32_t value) {
	ti81_pci_config_write(bus, dev_fn, where, 4, value);
	return PCIUTILS_SUCCESS;
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
	printk("%s: cmd_status=%08lx\n", __func__, REG_LOAD(TI81_PCI_CMD_STATUS));

	return 0;
}
