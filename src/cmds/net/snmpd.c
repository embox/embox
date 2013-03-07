/**
 * @file
 * @brief SNMP agent over SNMPv1
 *
 * @date 12.11.2012
 * @author Alexander Kalmuk
 */

#include <net/snmp.h>
#include <net/ip.h>
#include <net/mib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <embox/cmd.h>
#include <string.h>
#include <kernel/thread.h>
#include <unistd.h>
#include <errno.h>

#define SNMP_ADDR INADDR_ANY
#define SNMP_AGENT_PORT 161

#define MAX_SNMP_LEN 256
#define MAX_PDU_LEN 128

EMBOX_CMD(exec);

/* There is only one SNMP agent in system. So, we can make common socket */
static int sock;

static void print_usage(void) {
	printf("Usage: snmpd\n");
}

static void build_response(struct snmp_desc *snmp) {
	struct varbind *var, *nxt;
	mib_obj_t obj;

	snmp->pdu_type = PDU_GET_RESPONSE;

	dlist_foreach_entry(var, nxt, &snmp->varbind_list, link) {
		if ((obj = mib_obj_getbyoid(var->oid, var->oid_len))) {
			var->data = obj->data;
		} else {
			dlist_del(&var->link);
		}
	}
}

static void *snmp_agent(void *arg) {
	struct snmp_desc snmp;
	struct sockaddr_in addr;
	char snmpbuf[MAX_SNMP_LEN];
	char varbuf[MAX_PDU_LEN]; /* for received variables */
	socklen_t sklen = 14;

	while (1) {
		if (recvfrom(sock, snmpbuf, MAX_SNMP_LEN, 0, (struct sockaddr *)&addr, &sklen) > 0) {
			size_t len;
			snmp_parse(&snmp, snmpbuf, varbuf, MAX_PDU_LEN);
			build_response(&snmp);
			len = snmp_build(&snmp, snmpbuf);
			sendto(sock, snmpbuf, len, 0, (struct sockaddr *)&addr, sklen);
		}
	}

	return NULL;
}

static int exec(int argc, char **argv) {
	int opt;
	struct sockaddr_in our;
	struct thread *thread;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			printf("error: unsupported option %c\n", optopt);
			return -EINVAL;
		}
	}

	our.sin_family = AF_INET;
	our.sin_port= htons(SNMP_AGENT_PORT);
	our.sin_addr.s_addr = htonl(SNMP_ADDR);

	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("can not allocate socket\n");
		return -errno;
	}

	if (bind(sock, (struct sockaddr *)&our, sizeof(our)) < 0) {
		printf("can not bind socket\n");
		close(sock);
		return -errno;
	}

	thread_create(&thread, 0, snmp_agent, NULL);

	mib_init_all();

	printf("*SNMP agent started*\n");

	return 0;
}
