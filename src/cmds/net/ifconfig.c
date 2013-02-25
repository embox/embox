/**
 * @file
 * @brief Similar to the standard Unix ifconfig.
 *
 * @date 22.03.09
 * @author Anton Bondarev
 * @author Alexander Batyukov
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stddef.h>
#include <stdio.h>
#include <embox/cmd.h>
#include <net/inetdevice.h>
#include <net/etherdevice.h>
#include <net/util.h>

EMBOX_CMD(exec);

struct ifconfig_args {
	char with_a;
	char with_s;
	char with_v; /* TODO not used now */
	char with_arp, arp;
	char with_promisc, promisc;
	char with_allmulti, allmulti;
	char with_mcast; int mcast;
	char with_p2p; char p2p; struct in_addr p2p_addr; /* TODO setup p2p_addr (not used now) */
	char with_bcast; char bcast; struct in_addr bcast_addr;
	char with_iface; char iface[IFNAMSIZ];
	char with_addr; struct in_addr addr;
	char with_dstaddr; struct in_addr dstaddr; /* TODO setup dstaddr (not used now) */
	char with_netmask; struct in_addr netmask;
	char with_mtu; int mtu;
	char with_irq; int irq;
	char with_ioaddr; void *ioaddr;
	char with_txqueuelen; long int txqueuelen;
	char with_hw; unsigned char hw_addr[ETH_ALEN];
	char with_up_or_down; char up;
};

static int ifconfig_args_not_empty(struct ifconfig_args *args) {
	return args->with_up_or_down || args->with_arp || args->with_promisc
		|| args->with_allmulti || args->with_mtu || args->with_dstaddr
		|| args->with_netmask || args->with_irq || args->with_ioaddr
		|| args->with_bcast || args->with_p2p || args->with_hw || args->with_mcast
		|| args->with_addr || args->with_txqueuelen;
}

static int ifconfig_setup_iface(struct in_device *iface, struct ifconfig_args *args) {
	int ret;

	assert(iface != NULL);
	assert(args != NULL);

	if (args->with_arp) { /* set flag IFF_NOARP (default 0 means ARP enabled) */
		ret = (!args->arp ? etherdev_flag_up : etherdev_flag_down)(iface->dev, IFF_NOARP);
		if (ret != 0) return ret;
	}

	if (args->with_promisc) { /* set flag IFF_PROMISC (default 0) */
		ret = (args->promisc ? etherdev_flag_up : etherdev_flag_down)(iface->dev, IFF_PROMISC);
		if (ret != 0) return ret;
	}

	if (args->with_allmulti) { /* set flag IFF_ALLMULTI (default 0) */
		ret = (args->allmulti ? etherdev_flag_up : etherdev_flag_down)(iface->dev, IFF_ALLMULTI);
		if (ret != 0) return ret;
	}

	if (args->with_mcast) { /* set flag IFF_MULTICAST (default 0) */
		ret = (args->allmulti ? etherdev_flag_up : etherdev_flag_down)(iface->dev, IFF_MULTICAST);
		if (ret != 0) return ret;
	}

	if (args->with_p2p) { /* set flag IFF_POINTOPOINT */
		ret = (args->p2p ? etherdev_flag_up : etherdev_flag_down)(iface->dev, IFF_POINTOPOINT);
		if (ret != 0) return ret;
	}

	if (args->with_addr) { /* set new IP address to iface */
		ret = inet_dev_set_ipaddr(iface, args->addr.s_addr);
		if (ret != 0) return ret;
	}

	if (args->with_netmask) { /* set new mask to iface */
		ret = inet_dev_set_mask(iface, args->netmask.s_addr);
		if (ret != 0) return ret;
	}

	if (args->with_bcast) { /* set broadcast addr */
		if (args->bcast && args->bcast_addr.s_addr) {
			ret = inet_dev_set_bcast(iface, args->bcast_addr.s_addr);
			if (ret != 0) return ret;
		}
		else {
			ret = (args->bcast ? etherdev_flag_up : etherdev_flag_down)(iface->dev, IFF_BROADCAST);
			if (ret != 0) return ret;
		}
	}

	if (args->with_mtu) { /* set new MTU value */
		ret = etherdev_change_mtu(iface->dev, args->mtu);
		if (ret != 0) return ret;
	}

	if (args->with_irq) { /* set new IRQ number */
		ret = etherdev_set_irq(iface->dev, args->irq);
		if (ret != 0) return ret;
	}

	if (args->with_ioaddr) { /* set new base addr */
		ret = etherdev_set_baseaddr(iface->dev, (unsigned long)args->ioaddr);
		if (ret != 0) return ret;
	}

	if (args->with_txqueuelen) { /* set new max packet length */
		ret = etherdev_set_txqueuelen(iface->dev, args->txqueuelen);
		if (ret != 0) return ret;
	}

	if (args->with_hw) { /* set new MAC address to iface */
		ret = inet_dev_set_macaddr(iface, &args->hw_addr[0]);
		if (ret != 0) return ret;
	}

	if (args->with_up_or_down) { /* change device state */
		if (iface->dev->flags & IFF_UP) return 0; /* TODO remove this */
		ret = (args->up ? ifdev_up : ifdev_down)(&args->iface[0]);
		if (ret != 0) return ret;
	}

	return 0;
}

static int ifconfig_print_long_hdr(void) {
	/* NOTHING */
	return 0;
}

static int ifconfig_print_long_info(struct in_device *iface) {
	struct net_device_stats *stat;
	unsigned char mac[18];
	struct in_addr ip, bcast, mask;
	char s_ip[] = "xxx.xxx.xxx.xxx";
	char s_bcast[] = "xxx.xxx.xxx.xxx";
	char s_mask[] = "xxx.xxx.xxx.xxx";

	stat = iface->dev->netdev_ops->ndo_get_stats(iface->dev);

	printf("%s\tLink encap:", iface->dev->name);
	if (iface->dev->flags & IFF_LOOPBACK) {
		printf("Local Loopback");
	} else {
		macaddr_print(mac, iface->dev->dev_addr);
		printf("Ethernet  HWaddr %s", mac);
	}

	printf("\n\t");
	ip.s_addr = iface->ifa_address;
	mask.s_addr = iface->ifa_mask;
	strncpy(s_ip, inet_ntoa(ip), sizeof(s_ip));
	strncpy(s_mask, inet_ntoa(mask), sizeof(s_mask));
	if (iface->dev->flags & IFF_LOOPBACK) {
		printf("inet addr:%s  Mask:%s", s_ip, s_mask);
	} else {
		bcast.s_addr = iface->ifa_broadcast;
		strncpy(s_bcast, inet_ntoa(bcast), sizeof(s_bcast));
		printf("inet addr:%s  Bcast:%s  Mask:%s", s_ip, s_bcast, s_mask);
	}

	printf("\n\t");
	if (iface->dev->flags & IFF_UP) printf("UP ");
	if (iface->dev->flags & IFF_BROADCAST) printf("BROADCAST ");
	if (iface->dev->flags & IFF_DEBUG) printf("DEBUG ");
	if (iface->dev->flags & IFF_LOOPBACK) printf("LOOPBACK ");
	if (iface->dev->flags & IFF_POINTOPOINT) printf("POINTOPOINT ");
	if (iface->dev->flags & IFF_NOTRAILERS) printf("NOTRAILERS ");
	if (iface->dev->flags & IFF_RUNNING) printf("RUNNING ");
	if (iface->dev->flags & IFF_NOARP) printf("NOARP ");
	if (iface->dev->flags & IFF_PROMISC) printf("PROMISC ");
	if (iface->dev->flags & IFF_ALLMULTI) printf("ALLMULTI ");
	if (iface->dev->flags & IFF_MULTICAST) printf("MULTICAST ");
	printf(" MTU:%d  Metric:%d", iface->dev->mtu, 0);

	printf("\n\tRX packets:%ld errors:%ld dropped:%ld overruns:%ld frame:%ld",
			stat->rx_packets, stat->rx_err, stat->rx_dropped,
			stat->rx_over_errors, stat->rx_frame_errors);

	printf("\n\tTX packets:%ld errors:%ld dropped:%ld overruns:%ld carrier:%ld",
			stat->tx_packets, stat->tx_err, stat->tx_dropped, 0UL,
			stat->tx_carrier_errors);

	printf("\n\tcollisions:%ld txqueuelen:%ld",
			stat->collisions, iface->dev->tx_queue_len);

	printf("\n\tRX bytes:%ld (%ld MiB)  TX bytes:%ld (%ld MiB)",
			stat->rx_bytes, stat->rx_bytes / 1048576,
			stat->tx_bytes, stat->tx_bytes / 1048576);

	if (!(iface->dev->flags & IFF_LOOPBACK))
		printf("\n\tInterrupt:%d Base address:%p", iface->dev->irq, (void *)iface->dev->base_addr);

	printf("\n\n");

	return 0;
}

static int ifconfig_print_short_hdr(void) {
	printf("Iface  MTU Met  RX-OK RX-ERR RX-DRP RX-OVR  TX-OK TX-ERR TX-DRP TX-OVR Flg\n");
	return 0;
}

static int ifconfig_print_short_info(struct in_device *iface) {
	struct net_device_stats *stat;

	stat = iface->dev->netdev_ops->ndo_get_stats(iface->dev);

	printf("%-5s %5d %-2d %6lu %6lu %6lu %6lu %6lu %6lu %6lu %6lu ",
			iface->dev->name,
			iface->dev->mtu, 0,
			stat->rx_packets, stat->rx_err, stat->rx_dropped, stat->rx_over_errors,
			stat->tx_packets, stat->tx_err, stat->tx_dropped, 0UL);
	if (iface->dev->flags & IFF_UP) printf("U");
	if (iface->dev->flags & IFF_BROADCAST) printf("B");
	if (iface->dev->flags & IFF_DEBUG) printf("D");
	if (iface->dev->flags & IFF_LOOPBACK) printf("L");
	if (iface->dev->flags & IFF_RUNNING) printf("R");
	if (iface->dev->flags & IFF_NOARP) printf("O");
	if (iface->dev->flags & IFF_PROMISC) printf("P");
	if (iface->dev->flags & IFF_ALLMULTI) printf("A");
	if (iface->dev->flags & IFF_MULTICAST) printf("M");
	printf("\n");

	return 0;
}

static int ifconfig_show_one_iface(struct in_device *iface, char use_short_fmt) {
	int ret;

	ret = use_short_fmt ? ifconfig_print_short_hdr() : ifconfig_print_long_hdr();
	if (ret != 0) return ret;

	return use_short_fmt ? ifconfig_print_short_info(iface)
			: ifconfig_print_long_info(iface);
}

static int ifconfig_show_all_iface(char use_short_fmt) {
	int ret;
	struct in_device *iface;

	ret = use_short_fmt ? ifconfig_print_short_hdr() : ifconfig_print_long_hdr();
	if (ret != 0) return ret;

	for (iface = inet_dev_get_first_used(); iface != NULL;
			iface = inet_dev_get_next_used(iface)) {
		ret = use_short_fmt ? ifconfig_print_short_info(iface)
				: ifconfig_print_long_info(iface);
		if (ret != 0) return ret;
	}

	return 0;
}

static int exec(int argc, char *argv[]) {
	int i;
	struct in_device *iface;
	struct ifconfig_args args;

	memset(&args, 0, sizeof args);

	for (i = 1; i < argc; ++i) {
		if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
			printf("Usage:\n");
			printf("  %s [-a] [-v] [-s] <interface> [[<AF] <address>]\n", argv[0]);
			printf("  [[-]broadcast [<address>]]  [[-]pointopoint [<address>]]\n");
			printf("  [netmask <address>]  [dstaddr <address>]  [tunnel <address>]\n");
			printf("  [hw <HW> <address>]  [metric <NN>]  [mtu <NN>]\n");
			printf("  [[-]arp]  [[-]allmulti]  [multicast]  [[-]promisc]\n");
			printf("  [io_addr <NN>]  [irq <NN>]  [txqueuelen <NN>]\n");
			printf("  [up|down] ...\n");
			printf("\n");
			printf("  <HW>=Hardware Type.\n");
			printf("  List of possible hardware types:\n");
			printf("  loop (Local Loopback) ether (Ethernet)\n");
			printf("  <AF>=Address family. Default: inet\n");
			printf("  List of possible address families:\n");
			printf("  unix (UNIX Domain) inet (DARPA Internet) inet6 (IPv6)\n");
			return 0;
		}
		else if (!strcmp("-a", argv[i])) args.with_a = 1;
		else if (!strcmp("-s", argv[i])) args.with_s = 1;
		else if (!strcmp("-v", argv[i])) args.with_v = 1;
		else if (!args.with_iface) {
			args.with_iface = 1;
			strncpy(&args.iface[0], argv[i], ARRAY_SIZE(args.iface));
		}
		else if (!strcmp("up", argv[i]) || !strcmp("down", argv[i])) {
			args.with_up_or_down = 1;
			args.up = argv[i][0] == 'u';
		}
		else if (!strcmp("arp", argv[i]) || !strcmp("-arp", argv[i])) {
			args.with_arp = 1;
			args.arp = argv[i][0] != '-';
		}
		else if (!strcmp("promisc", argv[i]) || !strcmp("-promisc", argv[i])) {
			args.with_promisc = 1;
			args.promisc = argv[i][0] != '-';
		}
		else if (!strcmp("allmulti", argv[i])
				|| !strcmp("-allmulti", argv[i])) {
			args.with_allmulti = 1;
			args.allmulti = argv[i][0] != '-';
		}
		else if (!strcmp("mtu", argv[i])) {
			args.with_mtu = 1;
			if (sscanf(argv[++i], "%d", &args.mtu) != 1) {
				printf("%s: wrong mtu argument\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("dstaddr", argv[i])) {
			args.with_dstaddr = 1;
			if (!inet_aton(argv[++i], &args.dstaddr)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("netmask", argv[i])) {
			args.with_netmask = 1;
			if (!inet_aton(argv[++i], &args.netmask)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("broadcast", argv[i])
				|| !strcmp("-broadcast", argv[i])) {
			args.with_bcast = 1;
			args.bcast = argv[i][0] != '-';
			if (args.bcast && inet_aton(argv[i], &args.bcast_addr)) ++i;
		}
		else if (!strcmp("irq", argv[i])) {
			args.with_irq = 1;
			if (sscanf(argv[++i], "%d", &args.irq) != 1) {
				printf("%s: bad irq number\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("io_addr", argv[i])) {
			args.with_ioaddr = 1;
			if (sscanf(argv[++i], "%p", &args.ioaddr) != 1) {
				printf("%s: wrong start address format\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("pointopoint", argv[i])
				|| !strcmp("-pointopoint", argv[i])) {
			args.with_p2p = 1;
			args.p2p = argv[i][0] != '-';
			if (args.p2p && !inet_aton(argv[++i], &args.p2p_addr)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("hw", argv[i])) {
			args.with_hw = 1;
			if (strcmp("ether", argv[++i]) && strcmp("loop", argv[i])) {
				printf("%s: hardware class not supported\n", argv[i]);
				return -EINVAL;
			}
			if (!macaddr_scan((unsigned char *)argv[++i], &args.hw_addr[0])) {
				printf("%s: wrong hardware address format\n", argv[i]);
				return -EINVAL;
			}
		}
		else if (!strcmp("multicast", argv[i])
				|| !strcmp("-multicast", argv[i])) {
			args.with_mcast = 1;
			args.mcast = argv[i][0] != '-';
		}
		else if (!strcmp("txqueuelen", argv[i])) {
			args.with_txqueuelen = 1;
			if (sscanf(argv[++i], "%ld", &args.txqueuelen) != 1) {
				printf("%s: bad number\n", argv[i]);
				return -EINVAL;
			}
		}
		else {
			args.with_addr = 1;
			if (!strcmp("inet", argv[i]) || !strcmp("inet6", argv[i])
					|| !strcmp("unix", argv[i])) ++i;
			if (!inet_aton(argv[i], &args.addr)) {
				printf("%s: unknown host\n", argv[i]);
				return -EINVAL;
			}
		}
	}

	if (args.with_iface && !inet_dev_find_by_name(&args.iface[0]))
		ifdev_up(&args.iface[0]);

	if (args.with_iface) {
		iface = inet_dev_find_by_name(&args.iface[0]);
		if (iface == NULL) {
			printf("%s: unknown interface\n", &args.iface[0]);
			return -ENODEV;
		}
	}

	return ifconfig_args_not_empty(&args) ? ifconfig_setup_iface(iface, &args)
			: args.with_iface ? ifconfig_show_one_iface(iface, args.with_s)
			: ifconfig_show_all_iface(args.with_s);
}
