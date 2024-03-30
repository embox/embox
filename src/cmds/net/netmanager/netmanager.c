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
#include <getopt.h>
#include <arpa/inet.h>

#include <ifaddrs.h>

#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/util/macaddr.h>

#include <framework/mod/options.h>

#define BUFF_SZ      128
#define CONFIG_FILE  OPTION_STRING_GET(conf_file)

static void skip_commented_line(FILE *file) {
	int c;

	do {
		c = fgetc(file);
	} while (c != '\n' && c != EOF);
}

static int setup_static_config(FILE *input, char buf[BUFF_SZ], char *iface_name) {
	int err = 0;
	uint32_t if_ipv4_netmask;
	uint32_t if_ipv4_addr;
	int if_ipv4_net;
	char cmd_line[BUFF_SZ];
	char ipv4_addr[32] = "";
	char netmask[32] = "255.255.255.255";
	char gw[32] = "";
	char hw_addr[32] = "";
	char net[32] = "";
	int scanf_res = -1;

	while (fscanf(input, "%s", buf) != EOF && strcmp(buf, "iface")) {
		if (!strncmp(buf, "#", 1)) {
			skip_commented_line(input);
			continue;
		}

		scanf_res = -1;
		if (!strcmp(buf, "address")) {
			scanf_res = fscanf(input, "%s", ipv4_addr);
		} else if (!strcmp(buf, "netmask")) {
			scanf_res = fscanf(input, "%s", netmask);
		} else if (!strcmp(buf, "gateway")) {
			scanf_res = fscanf(input, "%s", gw);
		} else if (!strcmp(buf, "hwaddress")) {
			scanf_res = fscanf(input, "%s", hw_addr);
		} else {
			printf("WARNING: Unknown iface parameter: %s\n", buf);
			return EINVAL;
		}

		if (scanf_res != 1) {
			printf("Missing parameter for '%s'\n", buf);
			return EINVAL;
		}
	}

	if (1 != inet_pton(AF_INET, netmask, &if_ipv4_netmask)) {
		printf("netmanager: iface(%s) wrong netmask %s\n", iface_name, netmask);
		return EINVAL;
	}
	if (1 != inet_pton(AF_INET, ipv4_addr, &if_ipv4_addr)) {
		printf("netmanager: iface(%s) wrong ipaddr %s\n", iface_name, ipv4_addr);
		return EINVAL;
	}
	if_ipv4_net = if_ipv4_addr & if_ipv4_netmask;
	if (NULL == inet_ntop(AF_INET, &if_ipv4_net, net, sizeof(net))) {
		printf("netmanager: iface(%s) wrong net", iface_name);
		return EINVAL;
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

static int netmanager_is_dhcp_use(char *ifname) {
	char buf[BUFF_SZ] = "";
	FILE *input;
	char pattern[32] = "";
	int res = -1;
	int pat_len;

	input = fopen(CONFIG_FILE, "r");
	if (!input) {
		printf("Couldn't load file %s...\n", CONFIG_FILE);

		return -ENOENT;
	}
	strcat(pattern, "iface ");
	strcat(pattern, ifname);
	strcat(pattern, " inet dhcp");
	pat_len = strlen(pattern);

	while (1) {
		if (!fgets(buf, BUFF_SZ, input)) {
			break;
		}
		if (!strncmp(buf, pattern, pat_len)) {
			res = 1;
			break;
		}
		if (buf[0] == '#' && !strncmp(buf + 1, pattern, pat_len)) {
			res = 0;
			break;
		}
	}

	fclose(input);

	return res;
}

static in_addr_t *netmanager_get_gateway(char *ifname) {
	struct rt_entry *rt;

	for (rt = rt_fib_get_first(); rt != NULL; rt = rt_fib_get_next(rt)) {

		if ((rt->rt_gateway != 0) && !strcmp(ifname, rt->dev->name)) {
			return &rt->rt_gateway;
		}
	}

	return 0;
}


static int netmanager_store_config(char *ifname, int dhcp) {
	struct ifaddrs *i_ifa, *ifa = NULL;
	struct sockaddr *iaddr;
	char file_buf[256] = "";
	char buf[16];
	const char *tmp;
	struct in_device *iface_dev;

	if (-1 == getifaddrs(&ifa)) {
		return -1;
	}

	for (i_ifa = ifa; i_ifa != NULL; i_ifa = i_ifa->ifa_next) {

		if (0 == strcmp(i_ifa->ifa_name, ifname)) {
			break;
		}
	}

	if (!i_ifa) {
		freeifaddrs(ifa);
		return -1;
	}

	iaddr = i_ifa->ifa_addr;
	strcat(file_buf, "iface ");
	strcat(file_buf, ifname);
	strcat(file_buf, " inet static\n");
	strcat(file_buf, "\taddress ");
	tmp = inet_ntop(iaddr->sa_family,
		&((struct sockaddr_in *) iaddr)->sin_addr,
		buf,
		sizeof(buf));
	strcat(file_buf, tmp);
	strcat(file_buf, "\n\tnetmask ");
	tmp = inet_ntop(i_ifa->ifa_netmask->sa_family,
			&((struct sockaddr_in *) i_ifa->ifa_netmask)->sin_addr,
			buf,
			sizeof(buf));
	strcat(file_buf, tmp);
	strcat(file_buf, "\n\tgateway ");
	tmp = inet_ntop(iaddr->sa_family,
			netmanager_get_gateway(ifname),
			buf,
			sizeof(buf));
	strcat(file_buf, tmp);

	iface_dev = inetdev_get_by_name(i_ifa->ifa_name);
	if (iface_dev) {
		strcat(file_buf, "\n\thwaddress ");
		macaddr_print((unsigned char *)buf, iface_dev->dev->dev_addr);
		strcat(file_buf, buf);
	}

	strcat(file_buf, "\n\n");
	switch(dhcp) {
	case 0:
		strcat(file_buf, "#iface ");
		strcat(file_buf, ifname);
		strcat(file_buf, " inet dhcp\n");
		break;
	case 1:
		strcat(file_buf, "iface ");
		strcat(file_buf, ifname);
		strcat(file_buf, " inet dhcp\n");
		break;
	default:
		break;
	}
	freeifaddrs(ifa);

	FILE *file = fopen(CONFIG_FILE, "w");
	if (!file) {
		printf("Couldn't write file %s...\n", CONFIG_FILE);

		return -ENOENT;
	}
	fwrite(file_buf, strlen(file_buf), 1, file);
	fclose(file);

	return 0;
}

int main(int argc, char **argv) {
	char buf[BUFF_SZ] = "";
	int err;
	FILE *input;
	char ifname[0x20];
	int c;

	while ((c = getopt(argc, argv, "ds:")) != -1) {
		switch (c) {
			case 'd':
				return netmanager_is_dhcp_use(argv[argc-1]);
			case 's':
				{
					int use_dhcp;
					use_dhcp = strtol(optarg, NULL, 0);
					netmanager_store_config(argv[argc-1], use_dhcp);
					return 0;
				}
			default:
				break;
		}
	}

	if (argc == 2) {
		strncpy(ifname, argv[1], sizeof(ifname)-1);
		ifname[sizeof(ifname)-1] = '\0';
	}

	input = fopen(CONFIG_FILE, "r");
	if (!input) {
		printf("Couldn't load file %s...\n", CONFIG_FILE);

		return -ENOENT;
	}

	while (1) {
		/* iface eth0 inet static */
		if (strcmp(buf, "iface")) {
			if (!strncmp(buf, "#", 1)) {
				skip_commented_line(input);
			}
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
				continue;
			}
			switch(err) {
				case -ENOENT:
					printf("'bootp' not found using static config\n");
					break;
				default:
					printf("BOOTP failed, using static config for %s...\n", ifname);
					break;
				}
			continue;
		}
		setup_static_config(input, buf, ifname);
	}
	fclose(input);

	return 0;
}
