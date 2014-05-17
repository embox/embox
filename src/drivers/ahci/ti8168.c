#include <embox/unit.h>
#include <drivers/ahci.h>
#include <hal/reg.h>

EMBOX_UNIT_INIT(ahci_ti8168_init);

static struct ahci_hba ti8168_hba= {
	.base_addr = 0x4A140000,
};

int ahci_ti8168_init(void) {
	/* Power on device */
	REG_ORIN(0x48180000 + 0x560, 2);

	register_ahci_hba(&ti8168_hba);
	return 0;
}
