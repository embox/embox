/**
 * @file
 * @brief Configure network interfaces
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-01-19
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFF_SZ 128
#define CONFIG_FILE "network"

void print_usage() {
	printf("USAGE: netmanager [iface]\n");
}

static int setup_static_config(FILE *input, char buf[BUFF_SZ], char *iface_name) {
	int err = 0;
	int if_netmask;
	int if_ipv4_addr;
	int if_net;
	char cmd_line[BUFF_SZ];
	char ipv4_addr[32] = "";
	char netmask[32] = "255.255.255.255";
	char gw[32] = "";
	char hw_addr[32] = "";
	char net[32] = "";

	while (fscanf(input, "%s", buf) != EOF && strcmp(buf, "iface")) {
		if (!strcmp(buf, "address"))
			fscanf(input, "%s", ipv4_addr);
		else if (!strcmp(buf, "netmask"))
			fscanf(input, "%s", netmask);
		else if (!strcmp(buf, "gateway"))
			fscanf(input, "%s", gw);
		else if (!strcmp(buf, "hwaddress"))
			fscanf(input, "%s", hw_addr);
		else
			printf("WARNING: Unknown iface parameter: %s\n", buf);
	}

	if (1 != inet_pton(AF_INET, netmask, &if_netmask)) {
		printf("netmanager: iface(%s) wrong netmask %s", iface_name, netmask);
	}
	if (1 != inet_pton(AF_INET, ipv4_addr, &if_ipv4_addr)) {
		printf("netmanager: iface(%s) wrong ipaddr %s", iface_name, ipv4_addr);
	}
	if_net = if_ipv4_addr & if_netmask;
	if (NULL == inet_ntop(AF_INET, &if_net, net, sizeof(net))) {
		printf("netmanager: iface(%s) wrong net", iface_name);
	}

	strcpy(cmd_line, "ifconfig ");
	strcat(cmd_line, iface_name);
	strcat(cmd_line, " ");
	strcat(cmd_line, ipv4_addr);

	if (netmask[0]) {
		strcat(cmd_line, " netmask ");
		strcat(cmd_line, netmask);
	}

	if (hw_addr[0]) {
		strcat(cmd_line, " hw ether ");
		strcat(cmd_line, hw_addr);
	}

	strcat(cmd_line, " up");

	if ((err = system(cmd_line)))
		return err;

	/* route */
	strcpy(cmd_line, "route add ");
	strcat(cmd_line, net);
	strcat(cmd_line, " netmask ");
	strcat(cmd_line, netmask);
	strcat(cmd_line, " ");
	strcat(cmd_line, iface_name);

	if ((err = system(cmd_line)))
		return err;

	if (gw[0]) {
		strcpy(cmd_line, "route add default gw ");
		strcat(cmd_line, gw);
		strcat(cmd_line, " ");
		strcat(cmd_line, iface_name);

		if ((err = system(cmd_line)))
			return err;
	}

	return 0;
}

int main(int argc, char **argv) {
	char buf[BUFF_SZ] = "bootpc ";
	int err;
	FILE *input;
	char ifname[0x20];

	if (argc == 2) {
		strncpy(ifname, argv[1], sizeof(ifname)-1);
		ifname[sizeof(ifname)-1] = '\0';
	}

	input = fopen(CONFIG_FILE, "r");
	if (!input) {
		printf("Couldn't load file %s...\n", argv[1]);

		return -ENOENT;
	}

	while (1) {
		/* iface eth0 inet static */
		if (strcmp(buf, "iface")) {
			if (fscanf(input, "%s", buf) == EOF) {
				break;
			} else {
				continue;
			}
		}
		fscanf(input, "%s", buf);
		if (argc < 2) {
			strncpy(ifname, buf, sizeof(ifname)-1);
			ifname[sizeof(ifname)-1] = '\0';
		}
		if (strcmp(buf, ifname)) {
			/* Wrong iface*/
			continue;
		}
		/* found interface */
		fscanf(input, "%s", buf);
		if (strcmp(buf, "inet")) {
			/* now supported only IP interfaces*/
			printf("now supported only 'inet' interfaces\n");
			return -ENOTSUP;
		}
		fscanf(input, "%s", buf);
		if (strcmp(buf, "static")) {
			/* dynamic (bootp) setup */
			buf[0] = '\0';
			strcat(buf, "bootpc ");
			strncat(buf, ifname, sizeof(buf)-strlen(buf)-1);
			if (0 == (err = system(buf))) {
				return 0;
			}
			switch(err) {
				case -ENOENT:
					printf("'bootp' not found using static config\n");
					break;
				default:
					printf("BOOTP failed, loading default config for %s...\n", argv[1]);
					break;
				}
		}
		setup_static_config(input, buf, ifname);
	}

	return 0;
}
