/**
 * @file
 * @brief MIB object: mac address
 *
 * @date 14.11.2012
 * @author Alexander Kalmuk
 */

#include <net/lib/snmp_mib.h>
#include <embox/unit.h>
#include <net/inetdevice.h>
#include <net/l2/ethernet.h>

static struct obj_data mac;

static void mib_mac(void) {
	struct in_device *in_dev = NULL;
	char oid[] = {0x2b, 6, 1, 2, 1, 2, 2, 1, 6, 2};
	mib_obj_t obj = mib_obj_addbyoid(oid, 10);

	in_dev = inetdev_get_by_name("eth0");
	if (in_dev) {
		obj->data = &mac;
		obj->name = "embox_eth0_mac";
		obj->data->data = in_dev->dev->dev_addr;
		obj->data->type = PDU_STRING;
		obj->data->datalen = ETH_ALEN;
	}
}

MIB_OBJECT_REGISTER(mib_mac);
