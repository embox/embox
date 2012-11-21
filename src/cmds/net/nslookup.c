/**
 * @file
 * @brief name server lookup
 *
 * @date 31.08.12
 * @author Ilia Vaprol
 */
#include <embox/cmd.h>
#include <net/dns.h>
#include <stdio.h>
#include <arpa/inet.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int ret;
	struct dns_result result;
	struct dns_rr *rr;
	size_t i;

	if (argc != 2) {
		printf("Usage: %s name\n", argv[0]);
		return -1;
	}

	ret = dns_query(argv[1], DNS_RR_TYPE_A, DNS_RR_CLASS_IN, &result);
	if (ret != 0) {
		return ret;
	}

	printf("Server: %s\n", MODOPS_DNS_NAMESERVER);
	printf("Address: %s#%d\n", MODOPS_DNS_NAMESERVER, DNS_PORT_NUMBER);

	printf("\nAnswers:\n");
	for (i = 0, rr = result.an; i < result.ancount; ++i, ++rr) {
		if (rr->rtype == DNS_RR_TYPE_A) {
			printf("Name: %s\tAddress: %s\n", &rr->rname[0],
					inet_ntoa(*(struct in_addr *)&rr->rdata.a.address[0]));
		}
	}

	printf("\nAuthoritative nameservers:\n");
	for (i = 0, rr = result.ns; i < result.nscount; ++i, ++rr) {
		printf("Name: %s\tNameserver: %s\n", &rr->rname[0],
				&rr->rdata.ns.nsdname[0]);
	}

	dns_result_free(&result);

	return 0;
}
