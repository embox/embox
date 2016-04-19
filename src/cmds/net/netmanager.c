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

#define BUFF_SZ 128
#define CONFIG_FILE "network"

void print_usage() {
	printf("USAGE: netmanager [iface]\n");
}

static int setup_static_config(FILE *input, char buf[BUFF_SZ], char *iface_name) {
	int err = 0;;
	char ipv4_addr[BUFF_SZ] = "";
	char netmask[BUFF_SZ] = "";
	char gw[BUFF_SZ] = "";
	char hw_addr[BUFF_SZ] = "";

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

	strcpy(buf, "ifconfig ");
	strcat(buf, iface_name);
	strcat(buf, " ");
	strcat(buf, ipv4_addr);

	if (netmask[0]) {
		strcat(buf, " netmask ");
		strcat(buf, netmask);
	}

	if (hw_addr[0]) {
		strcat(buf, " hw ether ");
		strcat(buf, hw_addr);
	}

	strcat(buf, " up");

	if ((err = system(buf)))
		return err;

	if (gw[0]) {
		strcpy(buf, "route add default gw ");
		strcat(buf, gw);
		strcat(buf, " ");
		strcat(buf, iface_name);

		if ((err = system(buf)))
			return err;
	}
	return 0;
}

int main(int argc, char **argv) {
	char buf[BUFF_SZ] = "bootpc ";
	int err;
	FILE *input;

	if (argc != 2) {
		print_usage();
		return -EINVAL;
	}

	input = fopen(CONFIG_FILE, "r");
	if (!input) {
		printf("Couldn't load file %s...\n", argv[1]);

		return -ENOENT;
	}

	while (fscanf(input, "%s", buf) != EOF) {
		/* iface eth0 inet static */
		if (strcmp(buf, "iface")) {
			continue;
		}
		fscanf(input, "%s", buf);
		if (strcmp(buf, argv[1])) {
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
			strncat(buf, "bootpc ", sizeof(buf));
			strncat(buf, argv[1], sizeof(buf));
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
		setup_static_config(input, buf, argv[1]);
	}

	return 0;
}
