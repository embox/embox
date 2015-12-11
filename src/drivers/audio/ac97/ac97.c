/**
 * @file
 * @brief Driver for AC97
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-27
 *
 * In this implementation, there is no right/left channel control,
 * volume is always the same on all inputs and outputs
 */

#include <unistd.h>

#include <asm/io.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/audio/ac97.h>
#include <drivers/audio/portaudio.h>

static int ac97_bus;
static int ac97_devfn;
static int ac97_bar;

/**
 * @brief Reset registers to default values
 *
 * @return Negative error code
 */
int ac97_reset(void) {
	/* Actually we can wrute anything to
	 * this register to reset codec */
	out16(0xFFFF, ac97_bar + AC97_RESET);

	return 0;
}

/**
 * @brief Changle volume settings
 *
 * @param vol New volume from 0x0...0x3F range
 *
 * @return Negative error code
 */
int ac97_set_vol(int vol) {
	assert(vol <= 0x3F);
	assert(vol >= 0);

	out16(vol | (vol << 8), ac97_bar + AC97_MASTER);
	out16(vol | (vol << 8), ac97_bar + AC97_HEADPHONE);
	out16(vol | (vol << 8), ac97_bar + AC97_MASTER_MONO);

	return 0;
}

/**
 * @brief Get master volume value
 *
 * @return Master volume value from 0x0...0x3F range
 */
int ac97_get_vol(void) {
	return 0x3F & in16(ac97_bar + AC97_MASTER);
}

/**
 * @brief Initialize codec, setup static values and so on
 *
 * @param pci_dev PCI device of Intel Audio Controller
 * TODO handle other sound cards
 *
 * @return Negative error number
 */
int ac97_init(struct pci_slot_dev *pci_dev) {
	ac97_bus   = pci_dev->busn;
	ac97_devfn = PCI_DEVFN(pci_dev->slot, pci_dev->func);
	ac97_bar   = pci_dev->bar[0] & 0xFFFFFFFC;

	ac97_reset();
	/* Set maximum master volume */
	out16(0x3F3F, ac97_bar + AC97_MASTER);
	out16(0x0000, ac97_bar + AC97_PCM);

	//out16(0x3760, ac97_bar + AC97_DAC_SLOT_MAP);
	//out16(0x3600, ac97_bar + AC97_ADC_SLOT_MAP);

	//out16(0x0001, ac97_bar + 0x24);
	out16(0xFFFF, ac97_bar + AC97_RESET);

	return 0;
}
