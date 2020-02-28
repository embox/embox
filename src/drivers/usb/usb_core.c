/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.10.2013
 */

#include <errno.h>
#include <string.h>
#include <util/member.h>
#include <util/dlist.h>
#include <util/log.h>
#include <mem/misc/pool.h>
#include <kernel/panic.h>
#include <kernel/thread/thread_sched_wait.h>
#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb.h>

#include <mem/vmem.h>
#include <module/embox/arch/mmu.h>

static DLIST_DEFINE(usb_hcds_list);

POOL_DEF(usb_hcds, struct usb_hcd, USB_MAX_HCD);
POOL_DEF_ATTR(usb_requests, struct usb_request, USB_MAX_REQ, __attribute__((aligned(MMU_PAGE_SIZE))));

static struct usb_request *usb_request_alloc(struct usb_endp *endp) {
	struct usb_request *req;

	req = pool_alloc(&usb_requests);
	memset(req, 0, sizeof *req);
	req->endp = endp;

	usb_queue_link_init(&req->req_link);

	return req;
}

struct usb_request *usb_endp_request_alloc(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd, void *arg, unsigned token,
		void *buf, size_t len) {
	struct usb_request *req;

	req = usb_request_alloc(endp);
	assert(req);

	req->endp = endp;
	req->token = token;
	req->buf = buf;
	req->len = len;
	req->notify_hnd = notify_hnd;
	req->hnd_data = arg;

	waitq_init(&req->wq);

	return req;
}

static void usb_request_free(struct usb_request *req) {
	pool_free(&usb_requests, req);
}

const struct usb_desc_endpoint usb_desc_endp_control_default = {
	.b_length = sizeof(struct usb_desc_endpoint),
	.b_desc_type = USB_DESC_TYPE_ENDPOINT,
	.b_endpoint_address = 0,
	.bm_attributes = 0,
	.w_max_packet_size = 8,
	.b_interval = 0,
};

static void usb_build_control_header(struct usb_request *req, uint8_t req_type,
		uint8_t request, uint16_t value, uint16_t index, uint16_t count,
		void *data) {
	char *rbuf = (char *) &req->ctrl_header;

	assert(req->endp->type == USB_COMM_CONTROL);

	rbuf[0] = req_type;
	rbuf[1] = request;
	rbuf[2] = value & 0xff;
	rbuf[3] = value >> 8;
	rbuf[4] = index & 0xff;
	rbuf[5] = index >> 8;
	rbuf[6] = count & 0xff;
	rbuf[7] = count >> 8;
}

static inline struct usb_request *usb_link2req(struct usb_queue_link *ul) {
	return member_cast_out(ul, struct usb_request, req_link);
}

static int usb_endp_do_req(struct usb_endp *endp) {
	struct usb_queue_link *l;
	struct usb_request *req;
	struct usb_hcd *hcd;

	l = usb_queue_peek(&endp->req_queue);
	if (!l) {
		return 0;
	}

	req = usb_link2req(l);

	hcd = req->endp->dev->hcd;
	hcd->ops->request(req);

	return 0;
}

static int usb_endp_request(struct usb_endp *endp, struct usb_request *req) {
	bool endp_busy;
	struct usb_dev *dev = endp->dev;

	/* TODO think about safe context for this function,
	 * requests on detached devices should be prevented
	 * by upper level.
	 * assert(endp->dev->plug_state != USB_DEV_DETACHED);
	 */

	if (is_root_hub(dev)) {
		return dev->hcd->ops->root_hub_control(req);
	}

	endp_busy = usb_queue_add(&endp->req_queue, &req->req_link);
	if (!endp_busy) {
		return usb_endp_do_req(endp);
	}

	return 0;
}

static void usb_request_remove(struct usb_request *req, bool fire_handler) {
	struct usb_endp *endp = req->endp;
	int ret;

	if (req->notify_hnd && fire_handler) {
		req->notify_hnd(req, req->hnd_data);
	}

	ret = usb_queue_remove(&endp->req_queue, &req->req_link);
	usb_request_free(req);

	if (ret) {
		usb_endp_do_req(endp);
	}
}

void usb_request_complete(struct usb_request *req) {

	/* TODO reset failed request */

	if (req->req_stat != USB_REQ_NOERR) {
		log_error("usb_request %p: failed\n", req);
	}

	usb_request_remove(req, req->req_stat == USB_REQ_NOERR);
}

static void usb_endp_cancel(struct usb_endp *endp) {
	struct usb_queue_link *ul, *first;

	first = usb_queue_peek(&endp->req_queue);

	for (ul = usb_queue_last(&endp->req_queue);
			ul != first;
			ul = usb_queue_last(&endp->req_queue)) {
		struct usb_request *req = usb_link2req(ul);

		/* FIXME handler should be fired */
		usb_request_remove(req, false);
	}
}

void usb_dev_request_delete(struct usb_dev *dev) {
	int i;

	for (i = 0; i < dev->endp_n; i++) {
		usb_endp_cancel(dev->endpoints[i]);
	}
}

static unsigned short usb_endp_dir_token_map(struct usb_endp *endp) {
	switch (endp->direction) {
	case USB_DIRECTION_IN:
		return USB_TOKEN_IN;
	case USB_DIRECTION_OUT:
		return USB_TOKEN_OUT;
	default:
		break;
	}
	panic("%s: can't map unidirection endpoint to token\n", __func__);
}

static struct usb_request *usb_alloc_control_request(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd, void *arg,
		uint8_t req_type, uint8_t request, uint16_t value,
		uint16_t index, uint16_t count, void *data) {
	struct usb_request *req = NULL;
	int dir;

	assert(usb_endp_type(endp) == USB_COMM_CONTROL);
	dir = (req_type & USB_DIR_IN) ? USB_TOKEN_IN : USB_TOKEN_OUT;
	// TODO handle detached?
	req = usb_endp_request_alloc(endp, notify_hnd, arg,
			USB_TOKEN_SETUP | dir, data, count);
	if (!req) {
		return NULL;
	}
	usb_build_control_header(req, req_type, request,
		value, index, count, data);

	return req;
}

/* Sends URB and exits immediately, notify_hnd() will be called later. */
int usb_endp_control(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd, void *arg,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data) {
	struct usb_request *req;
	req = usb_alloc_control_request(endp, notify_hnd, arg,
			req_type, request, value, index, count, data);
	if (!req) {
		return -ENOMEM;
	}
	return usb_endp_request(endp, req);
}

static void usb_endp_req_wait_handler(struct usb_request *req, void *arg) {
	waitq_wakeup(&req->wq, 0);
}

/* Sends URB and waits until it finished. */
int usb_endp_control_wait(struct usb_endp *endp,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data, int timeout) {
	struct usb_request *req;
	struct waitq_link wl;
	int ret;

	req = usb_alloc_control_request(endp, usb_endp_req_wait_handler, NULL,
			req_type, request, value, index, count, data);
	if (!req) {
		return -ENOMEM;
	}

	waitq_link_init(&wl);
	waitq_wait_prepare(&req->wq, &wl);

	ret = usb_endp_request(endp, req);
	if (ret) {
		return ret;
	}
	ret = sched_wait_timeout(timeout, NULL);
	if (ret) {
		return ret;
	}

	return req->req_stat;
}

int usb_endp_bulk(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len) {
	struct usb_request *req;

	assert(usb_endp_type(endp) == USB_COMM_BULK);

	req = usb_endp_request_alloc(endp, notify_hnd, NULL,
			usb_endp_dir_token_map(endp), buf, len);

	return usb_endp_request(endp, req);
}

int usb_endp_bulk_wait(struct usb_endp *endp, void *buf,
	    size_t len, int timeout) {
	struct usb_request *req;
	struct waitq_link wl;
	int ret;

	assert(usb_endp_type(endp) == USB_COMM_BULK);

	req = usb_endp_request_alloc(endp, usb_endp_req_wait_handler, NULL,
			usb_endp_dir_token_map(endp), buf, len);

	waitq_link_init(&wl);
	waitq_wait_prepare(&req->wq, &wl);

	ret = usb_endp_request(endp, req);
	if (ret) {
		return ret;
	}
	ret = sched_wait_timeout(timeout, NULL);
	if (ret) {
		return ret;
	}

	return req->req_stat;
}

int usb_endp_interrupt(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len) {
	struct usb_request *req;

	assert(usb_endp_type(endp) == USB_COMM_INTERRUPT);

	req = usb_endp_request_alloc(endp, notify_hnd, NULL,
			usb_endp_dir_token_map(endp), buf, len);

	return usb_endp_request(endp, req);
}

struct usb_hcd *usb_hcd_alloc(struct usb_hcd_ops *ops, void *args) {
	struct usb_hcd *hcd = pool_alloc(&usb_hcds);

	if (!hcd) {
		return NULL;
	}

	hcd->ops = ops;
	index_init(&hcd->enumerator, 1, USB_HC_MAX_DEV, &hcd->idx_data);

	dlist_head_init(&hcd->lnk);

	if (ops->hcd_hci_alloc) {
		hcd->hci_specific = ops->hcd_hci_alloc(hcd, args);
		if (!hcd->hci_specific) {
			pool_free(&usb_hcds, hcd);
			return NULL;
		}
	}

	return hcd;
}

void usb_hcd_free(struct usb_hcd *hcd) {
	if (hcd->ops->hcd_hci_free) {
		hcd->ops->hcd_hci_free(hcd, hcd->hci_specific);
	}
	pool_free(&usb_hcds, hcd);
}

#ifndef NOMMU
static void usb_pool_set_nocache(struct pool *p) {
	log_debug("pool_addr = 0x%08x, pool_size = %d", p->memory, p->pool_size);
	vmem_set_flags(vmem_current_context(),
	        (mmu_vaddr_t) p->memory,
	        p->pool_size,
	        PROT_WRITE | PROT_READ | PROT_NOCACHE);
}
#endif

int usb_hcd_register(struct usb_hcd *hcd) {
	int ret;

	assert(hcd);

#ifndef NOMMU
	usb_pool_set_nocache(&usb_requests);
#endif

	dlist_head_init(&hcd->lnk);
	dlist_add_next(&hcd->lnk, &usb_hcds_list);

	assert(hcd->ops);
	assert(hcd->ops->hcd_start);
	assert(hcd->ops->hcd_stop);
	assert(hcd->ops->root_hub_control);
	assert(hcd->ops->request);

	if ((ret = hcd->ops->hcd_start(hcd))) {
		return ret;
	}

	return 0;
}
