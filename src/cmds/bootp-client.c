/*
 * @brief Simple BOOTP client
 *
 * @date 30.07.2012
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <string.h>
#include <kernel/time/timer.h>
#include <util/dlist.h>
#include <kernel/thread/event.h>
#include <kernel/thread/state.h>

#include <net/ip.h>
#include <net/in.h>
#include <net/socket.h>
#include <net/inetdevice.h>
#include <net/bootp.h>
#include <net/route.h>

#define DEFAULT_WAIT_TIME 3000

EMBOX_CMD(exec);

/* List contains events for each iface, that event_fire() on
 * BOOTP packet receiving */
DLIST_DEFINE(bootp_events);

struct bootp_event {
	struct event ev;
	struct dlist_head link;
};

static int bootp_sock;
static bool bootp_sock_in_use = false;

static void print_usage(void) {
	printf("Usage: bootp server");
}

static int bootp_client_receive(struct sock *sk, struct sk_buff *skb) {
	net_device_t *dev;
	struct bootp_event *bootp_ev, *next;

	netdev_foreach(dev) {
		if (!strncmp((void*)dev->dev_addr, (void*)skb->mac.raw, ETH_ALEN))
			break;
	}

	bootp_receive(sk, skb);

	dlist_foreach_entry(bootp_ev, next, &bootp_events, link) {
		if (!strcmp(bootp_ev->ev.name, dev->name))
			event_fire(&bootp_ev->ev);
	}

	return ENOERR;
}

static void *bootp_thd_run(void *data) {
	int ret;
	struct event *e = (struct event *)data;
	sched_sleep(e, DEFAULT_WAIT_TIME);
	thread_exit(&ret);
	return NULL;
}

int bootp_client(int bootp_server_timeout, in_device_t *dev) {
	int res;
	struct bootphdr bphdr;
	struct bootp_event e;
	struct sockaddr_in our, dst;
	struct thread *thread;
	/* XXX */
	unsigned int magic_cookie = htonl(0x63825363);

	if (!bootp_sock_in_use) {
		if (0 > (bootp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
			printf("Can't to alloc socket\n");
			return bootp_sock;
		}

		bootp_sock_in_use = true;
		dlist_init(&bootp_events);
		socket_set_encap_recv(bootp_sock, bootp_client_receive);

		our.sin_family = AF_INET;
		our.sin_port = htons((__u16)BOOTP_PORT_CLIENT);
		our.sin_addr.s_addr = htonl(INADDR_ANY);

		res = bind(bootp_sock, (struct sockaddr *)&our, sizeof(our));

		if (res < 0) {
			printf("error at bind()\n");
			goto error;
		}
	}

	event_init(&e.ev, dev->dev->name);

	dlist_head_init(&e.link);
	dlist_add_next(&e.link, &bootp_events);

	thread_create(&thread, 0, bootp_thd_run, &e.ev);
	thread_set_priority(thread, THREAD_PRIORITY_DEFAULT);

	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	dst.sin_port = htons((__u16)BOOTP_PORT_SERVER);

	memset(&bphdr, 0, sizeof(bphdr));

	bphdr.op = BOOTPREQUEST;
	bphdr.htype = HTYPE_ETHERNET;
	bphdr.hlen = ETH_ALEN;

	memcpy(bphdr.chaddr, dev->dev->dev_addr, 6);

	memcpy(bphdr.options, &magic_cookie, 4);

	bphdr.options[4] = TAG_GATEWAY;
	bphdr.options[5] = 0;
	bphdr.options[6] = TAG_END;

	rt_add_route(dev->dev, INADDR_ANY, INADDR_ANY, INADDR_ANY, 0);

	if (0 >= sendto(bootp_sock, (void*) &bphdr, sizeof(bphdr), 0, (struct sockaddr *)&dst, sizeof(dst))) {
		printf("Sending error\n");
		goto error;
	}

	while (!thread_state_exited(thread->state));

	rt_del_route(dev->dev, INADDR_ANY, INADDR_ANY, INADDR_ANY);

	res = ENOERR;

error:
	bootp_sock_in_use = false;
	close(bootp_sock);
	return res;
}

static int exec(int argc, char **argv) {
	int opt;
	in_device_t *dev;
	uint32_t bootp_server_timeout = DEFAULT_WAIT_TIME;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			/* FALLTHROUGH */
		default:
			print_usage();
			return 0;
		}
	}

	if (argc < 2) {
		printf("no interface was specified\n");
		return 0;
	}

	if (NULL == (dev = inet_dev_find_by_name(argv[argc - 1]))) {
		printf("can't find interface %s\n", argv[argc - 1]);
		return 0;
	}

	bootp_client(bootp_server_timeout, dev);

	return 0;
}
