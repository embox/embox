/**
 * @file
 * @brief TUN interface demo with ICMP echo responder
 *
 * @author Maxim Kamendov
 * @date 06.10.25
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <net/if.h>
#include <net/if_tun.h>
#include <net/util/checksum.h>

#define DEV_NAME           "tun0"
#define IFACE_INET_ADDRESS "10.0.3.1"
#define NETMASK            "255.255.255.0"
#define SUBNET             "10.0.3.0/24"

void setup(void) {
	char system_str[128];
	memset(system_str, 0, sizeof(system_str));
	strcpy(system_str, "ifconfig ");
	strcat(system_str, DEV_NAME);
	strcat(system_str, " ");
	strcat(system_str, IFACE_INET_ADDRESS);
	strcat(system_str, " netmask ");
	strcat(system_str, NETMASK);
	printf("%s\n", system_str);
	system(system_str);

	memset(system_str, 0, sizeof(system_str));
	strcpy(system_str, "route add ");
	strcat(system_str, SUBNET);
	strcat(system_str, " dev ");
	strcat(system_str, DEV_NAME);
	printf("%s\n", system_str);
	system(system_str);
}

int create_tun_device(char *dev_name, int flag) {
	struct ifreq ifr;
	int fd, err_code;

	fd = 1;
	if ((fd = open("/dev/tun0", O_RDWR)) < 0) {
		printf("Tun device fd creation error. (%d)\n", fd);
		return fd;
	}

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, DEV_NAME);
	ifr.ifr_flags |= flag;

	if ((err_code = ioctl(fd, TUNSETIFF, &ifr)) < 0) {
		printf("Tun device ioctl error. (%d)\n", err_code);
		close(fd);
		return err_code;
	}

	return fd;
}

/* This is an example app
*  It demonstrates the use of tun interface by responding to icmp messages that are routed to it */
int main(int argc, char *argv[]) {
	int tun_fd = create_tun_device(DEV_NAME, IFF_TUN);
	if (tun_fd < 0) {
		printf("tun_fd < 0\n");
	}
	printf("Tun device created successfully\n");

	setup();

	int ret_length = 0;
	uint8_t buf[1024];
	while (1) {
		/* Read message */
		ret_length = read(tun_fd, buf, sizeof(buf));
		if (ret_length < 0) {
			break;
		}
		uint8_t src_ip[4];
		uint8_t dst_ip[4];
		memcpy(src_ip, &buf[12], 4);
		memcpy(dst_ip, &buf[16], 4);
		printf("ICMP receive : %hhu.%hhu.%hhu.%hhu -> %hhu.%hhu.%hhu.%hhu "
		       "(%d)\n",
		    dst_ip[0], dst_ip[1], dst_ip[2], dst_ip[3], src_ip[0], src_ip[1],
		    src_ip[2], src_ip[3], ret_length);

		/* Create a reply message */
		memcpy(&buf[12], dst_ip, 4);
		memcpy(&buf[16], src_ip, 4);

		size_t iph_len = (buf[0] & 0xF) * 4;
		uint8_t *icmp_header =
		    buf
		    + iph_len;
		icmp_header[0] = 0;

		icmp_header[2] = 0;
		icmp_header[3] = 0;
		size_t icmp_len = ret_length - iph_len;
		uint16_t icmp_checksum = ptclbsum((uint16_t *)icmp_header, icmp_len);
		icmp_header[3] = (uint8_t)(icmp_checksum >> 8);
		icmp_header[2] = (uint8_t)icmp_checksum;

		uint8_t *ip_header = buf;
		ip_header[10] = 0;
		ip_header[11] = 0;
		uint16_t ip_checksum = ptclbsum(ip_header, iph_len);
		ip_header[11] = (uint8_t)(ip_checksum >> 8);
		ip_header[10] = (uint8_t)ip_checksum;

		/* Send reply */
		ret_length = write(tun_fd, buf, ret_length);
		printf("ICMP send : %hhu.%hhu.%hhu.%hhu -> %hhu.%hhu.%hhu.%hhu (%d)\n",
		    src_ip[0], src_ip[1], src_ip[2], src_ip[3], dst_ip[0], dst_ip[1],
		    dst_ip[2], dst_ip[3], ret_length);
	}

	return close(tun_fd);
}
