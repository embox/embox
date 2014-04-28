#include <embox/unit.h>
#include <drivers/ahci.h>

EMBOX_UNIT_INIT(ahci_init);

// TODO: should be configured externally
#define AHCI_MAX_HBA 4

static struct ahci_hba* ahci_hba_list[AHCI_MAX_HBA];
// spinlock declaration goes here;

int __register_ahci_hba(struct ahci_hba *dev)
{
	int i;

	for (i = 0; i < AHCI_MAX_HBA; i++)
		if (!ahci_hba_list[i])
			break;

	if (i == AHCI_MAX_HBA)
		return 1;

	ahci_hba_list[i] = dev;

	return 0;
}

int register_ahci_hba(struct ahci_hba *dev)
{
	// FIXME: Acquire spinlock
	int ret = __register_ahci_hba(dev);
	// FIXME: Release spinlock
	return ret;
}

int unregister_ahci_hba(struct ahci_hba *dev)
{
	//NYI
	return 1;
}

int enumerate_ahci_hba(int (*callback)(struct ahci_hba*, void*), void* cookie)
{
	int i;

	for (i = 0; i < AHCI_MAX_HBA; i++)
		if (ahci_hba_list[i])
		{
			int ret = callback(ahci_hba_list[i], cookie);
			if (ret)
				return ret;
		}
	return 0;
}

static int ahci_init(void)
{
	int i;

	for (i = 0; i < AHCI_MAX_HBA; i++)
		ahci_hba_list[i] = 0;

	return 0;
}
