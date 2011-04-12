/**
 * @file   lspci.c
 * @brief  show list pci's devices
 *
 * @date   11.04.11
 * @author Gleb Efimov
 */

#include <embox/cmd.h>
#include <types.h>
#include <lib/list.h>

#include <drivers/pci.h>
#include <drivers/pci_id.h>

EMBOX_CMD(exec);


static int exec(int argsc, char **argsv) {

	uint32_t 	bus, devfn, val;
	uint16_t 	device;
	uint8_t  	lc=1;

	for (bus = 0; bus < PCI_BUS_QUANTITY; ++bus) {
			for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
				pci_read_config32(bus, devfn, PCI_DEVICE_ID, &val);
				device = (val >> 16) & 0xffff;
				if (res != NULL)
				{
					TRACE("%d) %s\n", lc, res);
					++lc;
				}
			}
	}
	return 0;
}
