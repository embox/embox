/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 09.09.23
 */
#include <util/log.h>
#include <net/netdevice.h>
#include <drivers/amba_pnp.h>
#include <drivers/ambapp_id.h>

void __greth_dev_init(struct net_device *dev) {
	amba_dev_t amba_dev;
	int slotnum;

	slotnum = capture_amba_dev(&amba_dev, AMBAPP_VENDOR_GAISLER,
	    AMBAPP_DEVICE_GAISLER_ETHMAC, false, false);

	if (slotnum < 0) {
		log_error("Can't capture apb dev venID=0x%X, devID=0x%X",
		    AMBAPP_VENDOR_GAISLER, AMBAPP_DEVICE_GAISLER_ETHMAC);
	}
	else {
		dev->base_addr = amba_dev.bar[0].start;
		dev->irq = amba_dev.dev_info.irq;
	}
}
