/**
 * @file   lspci.c
 * @brief  show list pci's devices
 *
 * @date   11.04.11
 * @author Gleb Efimov
 */

#include <embox/cmd.h>
#include <lib/list.h>
#include <drivers/pci.h>

EMBOX_CMD(exec);

static int exec(int argsc, char **argsv) {
	list_head lc;
	list_for_each(&lc,pci_bus->pci_dev_list){
		TRACE("device - %d",pci_dev->device);
	}
}
