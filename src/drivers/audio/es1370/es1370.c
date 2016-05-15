/**
 * @file
 *
 * @date 14.05.2016
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <util/log.h>

#include <asm/io.h>

#include <drivers/pci/pci.h>
#include <kernel/irq.h>
#include <drivers/pci/pci_driver.h>

#include "es1370.h"

static uint32_t audio_base;


static irq_return_t es1370_interrupt(unsigned int irq_num, void *dev_id) {
	log_debug("New irq\n");
	return IRQ_HANDLED;
}

extern int ak4531_init(uint32_t base_addr);
static int es1370_hw_init(uint32_t base_addr) {
	uint32_t chip_sel_ctrl_reg;

	/* turn everything off */
	out32(0, base_addr + ES1370_REG_CONTROL);

	/* turn off legacy (legacy control is undocumented) */
	out32(0, base_addr + ES1370_REG_LEGACY);
	out32(0, base_addr + ES1370_REG_LEGACY + 4);

	/* turn off serial interface */
	out32(0, base_addr + ES1370_REG_SERIAL_CONTROL);

	/* enable the codec */
	chip_sel_ctrl_reg = in32(ES1370_REG_CONTROL);
	chip_sel_ctrl_reg |= CTRL_XCTL0 | CTRL_CDC_EN;
	out32(chip_sel_ctrl_reg, base_addr + ES1370_REG_CONTROL);

	ak4531_init(base_addr + ES1370_REG_CODEC);

	return 0;
}
static int es1370_init(struct pci_slot_dev *pci_dev) {
	int err;

	audio_base = pci_dev->bar[1] & 0xFF00;

	if ((err = irq_attach(pci_dev->irq, es1370_interrupt, IF_SHARESUP, 0, "iac"))) {
		log_error("can't alloc irq");
		return err;
	}

	es1370_hw_init(audio_base);

	return 0;
}

PCI_DRIVER("es1370 Audio Controller", es1370_init, 0x1274, 0x5000);
