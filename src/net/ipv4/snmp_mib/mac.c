/**
 * @file
 * @brief MIB object: mac address
 *
 * @date 14.11.2012
 * @author Alexander Kalmuk
 */

#include <net/mib.h>
#include <embox/unit.h>
#include <net/inetdevice.h>

static void mib_mac(void);

static struct obj_data mac;

static const struct mib_obj_register __mac = {
		.oid = {0x2b, 6, 1, 2, 1, 2, 2, 1, 6, 2}, /* oid */
		.init = mib_mac,						/* function for registering oid */
};

static void mib_mac(void) {
	in_device_t *in_dev = NULL;
	mib_obj_t obj = mib_obj_addbyoid(__mac.oid, 10);

	in_dev = inet_dev_find_by_name("eth0");
	if (in_dev) {
		obj->data = &mac;
		obj->name = "embox_eth0_mac";
		obj->data->data = in_dev->dev->dev_addr;
		obj->data->type = PDU_STRING;
		obj->data->datalen = ETH_ALEN;
	}
}

MIB_OBJECT_REGISTER(&__mac);
