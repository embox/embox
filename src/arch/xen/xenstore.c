#include "xenstore.h"
#include <xen/event_channel.h>
#include <xen/sched.h>
#include <barrier.h>
#include <string.h>
#include <kernel/printk.h>
#include <embox/unit.h>

static evtchn_port_t xenstore_evt;
extern char _text_vma;
static struct xenstore_domain_interface *xenstore;

/* Current request ID */
static int req_id = 0;

/* Initialise the XenStore */
int xenstore_init(start_info_t *start) {
	xenstore = (struct xenstore_domain_interface *)
			((machine_to_phys_mapping[start->store_mfn] << 12)
			 +
			((unsigned long) &_text_vma));
	xenstore_evt = start->store_evtchn;
	/* TODO: Set up the event channel */

	return 0;
}

/* Write a request to the back end */
static int xenstore_write_request(char *message, int length) {
	/* Check that the message will fit */
	if (length > XENSTORE_RING_SIZE)
	{
		return -1;
	}

	int i;
	for (i = xenstore->req_prod; length > 0; i++, length--)
	{
		/* Wait for the back end to clear enough space in the buffer */
		XENSTORE_RING_IDX data;
		do
		{
			data = i - xenstore->req_cons;
			mb();
		} while (data >= sizeof(xenstore->req));
		/* Copy the byte */
		int ring_index = MASK_XENSTORE_IDX(i);
		xenstore->req[ring_index] = *message;
		message++;
	}
	/* Ensure that the data really is in the ring before continuing */
	wmb();
	xenstore->req_prod = i;
	return 0;
}

/* Read a response from the response ring */
static int xenstore_read_response(char *message, int length) {
	int i;
	for (i = xenstore->rsp_cons; length > 0; i++, length--)
	{
		/* Wait for the back end put data in the buffer */
		XENSTORE_RING_IDX data;
		do
		{

			data = xenstore->rsp_prod - i;
			mb();
		} while (data == 0);
		/* Copy the byte */
		int ring_index = MASK_XENSTORE_IDX(i);
		*message = xenstore->rsp[ring_index];
		message++;
	}
	xenstore->rsp_cons = i;
	return 0;
}

#define NOTIFY() \
	do { \
		struct evtchn_send event; \
		event.port = xenstore_evt; \
		HYPERVISOR_event_channel_op(EVTCHNOP_send, &event); \
	} while (0)

#define IGNORE(n) \
	do { \
		char buffer[XENSTORE_RING_SIZE]; \
		xenstore_read_response(buffer, n); \
	} while (0)

/* Write a key/value pair to the XenStore */
int xenstore_write(char *key, char *value) {
	int key_length = strlen(key);
	int value_length = strlen(value);
	struct xsd_sockmsg msg;
	msg.type = XS_WRITE;
	msg.req_id = req_id;
	msg.tx_id = 0;
	msg.len = 2 + key_length + value_length;
	/* Write the message */
	xenstore_write_request((char *) &msg, sizeof(msg));
	xenstore_write_request(key, key_length + 1);
	xenstore_write_request(value, value_length + 1);
	/* Notify the back end */
	NOTIFY();
	xenstore_read_response((char *) &msg, sizeof(msg));
	IGNORE(msg.len);
	if (msg.req_id != req_id++)
	{
		return -1;
	}
	return 0;
}

/* Read a value from the store */
int xenstore_read(char *key, char *value, int value_length) {
	int key_length = strlen(key);
	struct xsd_sockmsg msg;
	msg.type = XS_READ;
	msg.req_id = req_id;
	msg.tx_id = 0;
	msg.len = 1 + key_length;
	/* Write the message */
	xenstore_write_request((char *) &msg, sizeof(msg));
	xenstore_write_request(key, key_length + 1);
	/* Notify the back end */
	NOTIFY();
	xenstore_read_response((char *) &msg, sizeof(msg));
	if (msg.req_id != req_id++)
	{
		IGNORE(msg.len);
		return -1;
	}
	/* If we have enough space in the buffer */
	if (value_length >= msg.len)
	{
		xenstore_read_response(value, msg.len);
		return msg.len;
	}
	/* Truncate */
	xenstore_read_response(value, value_length);
	IGNORE(msg.len - value_length);
	return -2;
}

int xenstore_ls(char *key, char *values, int value_length) {
	int key_length = strlen(key);
	struct xsd_sockmsg msg;
	msg.type = XS_DIRECTORY;
	msg.req_id = req_id;
	msg.tx_id = 0;
	msg.len = 1 + key_length;
	/* Write the message */
	xenstore_write_request((char *) &msg, sizeof(msg));
	xenstore_write_request(key, key_length + 1);
	/* Notify the back end */
	NOTIFY();
	xenstore_read_response((char *) &msg, sizeof(msg));
	if (msg.req_id != req_id++)
	{
		IGNORE(msg.len);
		return -1;
	}
	/* If we have enough space in the buffer */
	if (value_length >= msg.len)
	{
		xenstore_read_response(values, msg.len);
		return msg.len;
	}
	/* Truncate */
	xenstore_read_response(values, value_length);
	IGNORE(msg.len - value_length);
	return -2;
}
