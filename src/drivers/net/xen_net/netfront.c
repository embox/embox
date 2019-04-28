/**
 * @file 
 * @brief Frontend initialization
 *
 * @date 
 * @author 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <kernel/printk.h>
#include <defines/null.h>

#include <xenstore.h>

#include "netfront.h"

#define XS_MSG_LEN 256

static char _text;

static char memory_pages[2][PAGE_SIZE];

static grant_entry_v1_t gnttab_table[16];
static grant_ref_t ref = 0; // it's ok

static grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame,
		int readonly)
{
    gnttab_table[ref].frame = frame;
    gnttab_table[ref].domid = domid;
    // wmb();
    readonly *= GTF_readonly;
    gnttab_table[ref].flags = GTF_permit_access | readonly;

    return ref++;
}

static void xenstore_interaction(struct netfront_dev *dev, char **ip) {
	char xs_key[XS_MSG_LEN], xs_value[XS_MSG_LEN];
	int err;

	printk("Start transaction\n");

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/tx-ring-ref", dev->nodename);

	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->tx_ring_ref);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write tx ring-ref\n");
		return;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/rx-ring-ref", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->rx_ring_ref);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write rx ring-ref\n");
		return;
	}

#if 0
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/event-channel", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", dev->evtchn);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write event-channel");
		return;
	}
#endif

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/request-rx-copy", dev->nodename);
	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", 1);
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] Can not write request-rx-copy\n");
		return;
	}

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/state", dev->nodename);

	memset(xs_value, 0, XS_MSG_LEN);
	sprintf(xs_value, "%u", XenbusStateConnected);

#if 1
	err = xenstore_write(xs_key, xs_value);
	if (err) {
		printk("[PANIC!] can not switch state\n");
		return;
	}
#endif

	printk("End transaction\n");

	printk("backend %p %d %d\n", dev->backend, sizeof(dev->backend),
			sizeof(dev->nodename));
	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/backend", dev->nodename);
	xenstore_read(xs_key, dev->backend, sizeof(dev->backend));

	memset(xs_key, 0, XS_MSG_LEN);
	sprintf(xs_key, "%s/mac", dev->nodename);
	xenstore_read(xs_key, dev->mac, XS_MSG_LEN);

	if ((dev->backend == NULL) || (dev->mac == NULL)) {
		printk("[PANIC!] backend/mac failed\n");
		return;
	}

	printk("backend at %s\n", dev->backend);
	printk("mac is %s\n", dev->mac);

	{
		XenbusState state;
		int count = 0;

		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/state", dev->backend);

		// xenbus_watch_path_token(XBT_NIL, path, path, &dev->events);

		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		state = atoi(xs_value);
		while (count < 10 && state < XenbusStateConnected) {
			memset(xs_value, 0, XS_MSG_LEN);
			xenstore_read(xs_key, xs_value, XS_MSG_LEN);
			state = atoi(xs_value);
			sleep(1);
			++count;
		}

		if (state != XenbusStateConnected) {
			printk("[PANIC!] backend not avalable, state=%d\n", state);
			// xenbus_unwatch_path_token(XBT_NIL, path, path);
			return;
		}
#if 1
		memset(xs_key, 0, XS_MSG_LEN);
		sprintf(xs_key, "%s/ip", dev->backend);
		memset(xs_value, 0, XS_MSG_LEN);
		xenstore_read(xs_key, xs_value, XS_MSG_LEN);
		strcpy(*ip, xs_value);
#endif
	}

	printk("**************************\n");
	// unmask_evtchn(dev->evtchn);

	return;
}

struct netfront_dev *init_netfront(
	char *_nodename,
	void (*thenetif_rx)(unsigned char* data,
		int len, void* arg),
	unsigned char rawmac[6],
	char **ip
) {
	struct netif_tx_sring *txs;
	struct netif_rx_sring *rxs;

	txs = (struct netif_tx_sring *) memory_pages[0];
	rxs = (struct netif_rx_sring *) memory_pages[1];
	memset(txs, 0, PAGE_SIZE);
	memset(rxs, 0, PAGE_SIZE);

	SHARED_RING_INIT(txs);
	SHARED_RING_INIT(rxs);

	char nodename[256];
	static int netfrontends = 0;

	snprintf(nodename, sizeof(nodename), "device/vif/%d", netfrontends);

	struct netfront_dev *dev = NULL;
	dev = malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));
	dev->nodename = strdup(nodename);

	FRONT_RING_INIT(&dev->tx, txs, PAGE_SIZE);
	FRONT_RING_INIT(&dev->rx, rxs, PAGE_SIZE);

	dev->tx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(txs), 0);
	dev->rx_ring_ref = gnttab_grant_access(dev->dom, virt_to_mfn(rxs), 0);

	xenstore_interaction(dev, ip);

    if (rawmac)
        sscanf(dev->mac,"%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			   &rawmac[0],
			   &rawmac[1],
			   &rawmac[2],
			   &rawmac[3],
			   &rawmac[4],
			   &rawmac[5]);

	return dev;
}
