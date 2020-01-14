#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <linux/if_packet.h>

static char buf[4096];

void hex_print(char *buf, int len) {
	const int width = 16;
	for (int i = 0; i < len; ++i) {
		printf("%02hhx%c", buf[i], ((i+1) % width == 0) ? '\n' : ' ');
	}

	if ((len % width) != 0) {
		putchar('\n');
	}
	putchar('\n');
}

uint16_t cksum(unsigned char *buf, int len) {
	uint32_t sum = 0;
	for (int i = 0; i < len - 1; i += 2) {
		sum += *(uint16_t *)&buf[i];
	}
	if (len % 2) {
		sum += buf[len - 1];
	}
	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);
	return ~sum;
}

int main(int argc, char *argv[]) {
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (-1 == sock) {
		perror("sock");
		return 1;
	}

	struct sockaddr_ll sockaddr;
	socklen_t socklen;

	int len;
	while (0 < (socklen = sizeof(sockaddr),
			len = recvfrom(sock, buf, sizeof(buf), 0,
				(struct sockaddr *)&sockaddr, &socklen))) {

#if 0
		hex_print(buf, len);
#endif

		struct ether_header *eth = (struct ether_header *)buf;
		bool lo = true;
		for (int i = 0; i < ETH_ALEN; ++i) {
			if (eth->ether_dhost[i] != 0 || eth->ether_dhost[i] != 0) {
				lo = false;
				break;
			}
		}
		if (!lo) {
			continue;
		}

		if (ntohs(eth->ether_type) != ETHERTYPE_IP) {
			continue;
		}
		struct iphdr *ip = (struct iphdr *)((char*)eth + sizeof(*eth));

		if (ip->protocol != IPPROTO_ICMP) {
			continue;
		}
		struct icmphdr *icmp = (struct icmphdr *)((char*)ip + sizeof(*ip));

		if (icmp->type != ICMP_ECHO) {
			continue;
		}

		uint32_t t = ip->saddr;
		ip->saddr = ip->daddr;
		ip->daddr = t;

		icmp->type = ICMP_ECHOREPLY;
		icmp->checksum = 0;
		icmp->checksum = cksum((char *)icmp, buf + len - (char*)icmp);

		if (-1 == sendto(sock, buf, len, 0, (struct sockaddr*)&sockaddr, socklen)) {
			perror("sendto");
		}
	}
	perror("read");
	return 1;
}
