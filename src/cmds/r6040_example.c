/**
 * @file
 *
 * @date 03.07.11
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <drivers/r6040.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: r6040_example\n");
}

// Choose a random protocol number for our protocol
#define ETHER_PROTO 0x1234

// Structures for ethernet packets.
struct _eth_addr {
	unsigned short addr0;
	unsigned short addr1;
	unsigned short addr2;
};

struct _eth_pkt {
	struct _eth_addr dst;
	struct _eth_addr src;
	unsigned short proto;
	char data; // data block
};

static struct _eth_pkt* g_pkt = NULL;
static size_t r6040_len;

static struct _eth_addr my_addr;

static int check_pkt(void) {
	r6040_len = r6040_rx((unsigned char*) g_pkt, 1500);
	if (!r6040_len)
		return 0;
	// ignore broadcasts.
	if ((g_pkt->dst.addr0 == 0xffff)  &&
	    (g_pkt->dst.addr1 == 0xffff)  &&
	    (g_pkt->dst.addr2 == 0xffff))
		return 0;
	if (g_pkt->proto != ETHER_PROTO)
		return 0;
	return 1;
}

// return to sender
static void reverse_pkt(void) {
	my_addr = g_pkt->dst;
	g_pkt->dst = g_pkt->src;
	g_pkt->src = my_addr;
}

static void check_for_packets(void) {
	if (!check_pkt())
		return; // move along, nothing to see here...

	// print the string we received, let's hope it's null terminated
	printf("Got one packet: ");
	printf(&g_pkt->data);
	printf("\n");

	// ready the response.
	reverse_pkt();

	// copy something into the packet payload
	strcpy(&g_pkt->data, "Embox says 'hello'");

	// Send the response
	r6040_tx((unsigned char*)g_pkt, 64);
}

static int exec(int argc, char **argv) {
	int opt;
	int t_linkup = 0;

	getopt_init();
	do {
		opt = getopt(argc, argv, "h");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	printf("Embox Ethernet packet reception example.\n");
	// Wait up to 2 seconds for the PHY link to come up
	t_linkup = r6040_wait_linkup();
	if (t_linkup) { // link came up.
		printf("Cable is plugged in!\r\n");
	} else {
		printf("Plug in the ethernet cable and try again.\r\n");
		return 0;
	}
	r6040_init();
	printf("Waiting for packets...\r\n");
	while (1) {
		check_for_packets();
	}
	return 0;
}
