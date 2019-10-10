#include <embox/unit.h>
#include <drivers/ahci/ahci.h>
#include <hal/reg.h>
#include <kernel/printk.h>

#include <hal/mmu.h>
#include <drivers/common/memory.h>
#include <mem/vmem.h>

EMBOX_UNIT_INIT(ahci_ti8168_init);

static struct ahci_hba ti8168_hba= {
	.base_addr = 0x4A140000,
	.nports    = 2,
};

int ahci_ti8168_init(void) {
	/* Power on device */
	REG_ORIN(0x48180000 + 0x560, 2);

	REG_STORE(ti8168_hba.base_addr + 0x0c, 2);

	printk("Registering HBA\n");
	register_ahci_hba(&ti8168_hba);
	return 0;
}

PERIPH_MEMORY_DEFINE(ahci_ti8168, 0x4A140000, 0x1000);

PERIPH_MEMORY_DEFINE(ti816x_power, 0x48180000, 0x1000);
