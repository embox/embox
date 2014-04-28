#ifndef DRIVERS_AHCI_H_
#define DRIVERS_AHCI_H_

#include <stdint.h>

struct ahci_hba {
	void *base_addr;
};

int register_ahci_hba(struct ahci_hba*);
int unregister_ahci_hba(struct ahci_hba*);
int enumerate_ahci_hba(int (*)(struct ahci_hba*, void*), void*);

#endif
