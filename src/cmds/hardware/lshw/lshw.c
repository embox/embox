/**
 * @file lshw.c
 * 
 * @brief Show all hardware configuration and properties
 * 
 * @date 14.12.2020
 * @author Avinal Kumar
 */

#include <drivers/usb/usb.h>
#include <stdio.h>
#include <unistd.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>

#include <lib/libds/array.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <drivers/pci/pci_repo.h>

#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include <drivers/block_dev.h>

#include <lib/libcpu_info.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <net/inetdevice.h>
#include <net/netdevice.h>
#include <net/util/macaddr.h>
#include <netinet/in.h>

static void print_usage(void) {
	printf("Usage: lshw [-h]\n");
	printf("\t[-h]      - print this help\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
	print_usage();
}

/* cpuinfo implementation */
static void print_cpuinfo(void) {
	uint64_t cpu_cnt;
	struct cpu_info *cinfo = get_cpu_info();

	printf("CPU:\n");

	printf("\t%-20s %s\n", "CPU Vendor ID ", cinfo->vendor_id);

	for (int i = 0; i < cinfo->feature_count; i++) {
		printf("\t");
		printf("CPU %-16s %u",
				cinfo->feature[i].name,
				cinfo->feature[i].val
		);
		printf("\n");
	}

	cpu_cnt = get_cpu_counter();
	if (cpu_cnt) {
		printf("\tCurrent time stamp counter: %" PRIu64 "\n", cpu_cnt);
	}
	printf("\n\n");
}

/* cpuinfo implementation end */

/* lsusb implementation */
static void show_usb_dev(struct usb_dev *usb_dev) {
	printf("Bus %03d Device %03d ID %04x:%04x\n",
			usb_dev->bus_idx,
			usb_dev->addr,
			usb_dev->dev_desc.id_vendor,
			usb_dev->dev_desc.id_product
	);
}

static void show_usb_desc_device(struct usb_dev *usb_dev) {
	printf(
			" Device Descriptor:\n"
			"   b_length             %5u\n"
			"   b_desc_type          %5u\n"
			"   bcd_usb              %2x.%02x\n"
			"   b_dev_class          %5u\n"
			"   b_dev_subclass       %5u\n"
			"   b_dev_protocol       %5u\n"
			"   b_max_packet_size    %5u\n"
			"   id_vendor           0x%04x\n"
			"   id_product          0x%04x \n"
			"   bcd_device           %2x.%02x\n"
			"   i_manufacter         %5u\n"
			"   i_product            %5u\n"
			"   i_serial_number      %5u\n"
			"   b_num_configurations %5u\n\n",
			usb_dev->dev_desc.b_length, 
			usb_dev->dev_desc.b_desc_type,
			usb_dev->dev_desc.bcd_usb >> 8, 
			usb_dev->dev_desc.bcd_usb & 0xff,
			usb_dev->dev_desc.b_dev_class,
			usb_dev->dev_desc.b_dev_subclass,
			usb_dev->dev_desc.b_dev_protocol, 
			usb_dev->dev_desc.b_max_packet_size,
			usb_dev->dev_desc.id_vendor, 
			usb_dev->dev_desc.id_product,
			usb_dev->dev_desc.bcd_device >> 8, 
			usb_dev->dev_desc.bcd_device & 0xff,
			usb_dev->dev_desc.i_manufacter,
			usb_dev->dev_desc.i_product,
			usb_dev->dev_desc.i_serial_number,
			usb_dev->dev_desc.b_num_configurations
	);
}

static void show_usb_desc_interface(struct usb_dev *usb_dev) {
	if (!usb_dev->current_config->usb_iface[0]->iface_desc[0]) {
		printf(" Interface Descriptor:\n"
			   "   No interfaces\n"
		);
		return;
	}

	printf(
			" Interface Descriptor:\n"
			"   b_length             %5u\n"
			"   b_desc_type          %5u\n"
			"   b_interface_number   %5u\n"
			"   b_alternate_setting  %5u\n"
			"   b_num_endpoints      %5u\n"
			"   b_interface_class    %5u\n"
			"   b_interface_subclass %5u\n"
			"   b_interface_protocol %5u\n"
			"   i_interface          %5u\n",
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_length,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_desc_type,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_interface_number,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_alternate_setting,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_num_endpoints,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_interface_class,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_interface_subclass,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_interface_protocol,
			usb_dev->current_config->usb_iface[0]->iface_desc[0]->i_interface
	);
}

static void show_usb_desc_configuration(struct usb_dev *usb_dev) {
	struct usb_desc_configuration *config =
		(struct usb_desc_configuration *)usb_dev->current_config->config_buf;

	if (!config) {
		printf(" Configuration Descriptor:\n"
			   "   No configurations\n\n"
		);
		return;
	}

	printf(
			" Configuration Descriptor:\n"
			"   b_length             %5u\n"
			"   b_desc_type          %5u\n"
			"   w_total_length      0x%04x\n"
			"   b_num_interfaces     %5u\n"
			"   bConfigurationValue  %5u\n"
			"   i_configuration      %5u\n"
			"   bm_attributes         0x%02x\n"
			"   b_max_power          %5u\n\n",
			config->b_length, 
			config->b_desc_type,
			config->w_total_length,
			config->b_num_interfaces, 
			config->b_configuration_value,
			config->i_configuration,
			config->bm_attributes,
			config->b_max_power
	);	
}

static void print_lsusb_information(void) {
	struct usb_dev *usb_dev = NULL;

	printf("USB devices: \n");
	while ((usb_dev = usb_dev_iterate(usb_dev))) {
		show_usb_dev(usb_dev);
		show_usb_desc_device(usb_dev);
		show_usb_desc_configuration(usb_dev);
		show_usb_desc_interface(usb_dev);
	}
    printf("\n\n");
}
/* lsusb implementation end */

/* lspci implementation */
static inline char *pci_get_region_type(uint32_t region_reg) {
	if (region_reg & 0x1) {
		return "I/O";
	} else {
		return "Mem";
	}
}

static inline size_t pci_get_region_size(uint32_t region_reg) {
	if (0 == (region_reg & 0x1)) {
		return 1 << 24;
	} else {
		return 1 << 8;
	}
}

static void show_pci_device(struct pci_slot_dev *pci_dev) {
	printf("%02" PRId32 ":%2" PRIx8 ".%" PRId8 " (PCI dev %04X:%04X) [%d %d]\n"
				"\t %s: %s %s (rev %02d)\n",
				pci_dev->busn,
				pci_dev->slot,
				pci_dev->func,
				pci_dev->vendor,
				pci_dev->device,
				pci_dev->baseclass,
				pci_dev->subclass,
				find_class_name(pci_dev->baseclass, pci_dev->subclass),
				find_vendor_name(pci_dev->vendor),
				find_device_name(pci_dev->device),
				pci_dev->rev
	);

	int bar_num;
	printf("\t  IRQ number: %d\n", pci_dev->irq);

	for (bar_num = 0; bar_num < ARRAY_SIZE(pci_dev->bar); bar_num ++) {
		uintptr_t base_addr = pci_dev->bar[bar_num];
		if (0 == base_addr) {
			continue;
		}
		printf("\t  Region (%s): Base: 0x%" PRIXPTR " [0x%" PRIXPTR "]\n",
				pci_get_region_type(base_addr),
				base_addr & ~((1 << 4) - 1),
				(base_addr & ~((1 << 4) - 1)) +
				(pci_get_region_size(base_addr) - 1)
		);
	}
}

static void print_lspci_information(void) {
	struct pci_slot_dev *pci_dev;

	printf("PCI devices: \n");
    pci_foreach_dev(pci_dev) {
		show_pci_device(pci_dev);
	}
    printf("\n\n");
}
/* lspci implementation end */

/* lsblk implementation */
static const char *convert_unit(uint64_t *size) {
	const char *unit;

	if (*size < 1024) {
		unit = "B";
	}
	else if (*size < 1024 * 1024) {
		unit = "KB";
		*size = *size / 1024.0;
	}
	else if (*size < 1024ull * 1024 * 1024) {
		unit = "MB";
		*size = *size / (1024.0 * 1024);
	}
	else if (*size < 1024ull * 1024 * 1024 * 1024) {
		unit = "GB";
		*size = *size / (1024.0 * 1024 * 1024);
	} else {
		unit = "TB";
		*size = *size / (1024.0 * 1024 * 1024 * 1024);
	}
	
	return unit;
}

void print_lsblk_information(void) {
	int i;
	struct block_dev **bdevs;
	bdevs = get_bdev_tab();
	assert(bdevs);

	printf("Block devices: \n");
	printf("  ID |  NAME          |    SIZE    | TYPE\n");

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i] && !block_dev_parent(bdevs[i])) {
			int j;
			dev_t id;
			const char *name;
			const char *unit;
			uint64_t size;

			name = block_dev_name(bdevs[i]);
			size = block_dev_size(bdevs[i]);
			id = block_dev_id(bdevs[i]);

			unit = convert_unit(&size);
			printf("%4" PRIdMAX " | %6s         | %8" PRId64 "%s | %s\n",
			    (uintmax_t)id, name, size, unit, "disk");

			for (j = 0; j < MAX_BDEV_QUANTITY; j++) {
				if (bdevs[j] && (bdevs[i] == block_dev_parent(bdevs[j]))) {
					name = block_dev_name(bdevs[j]);
					size = block_dev_size(bdevs[j]);
					id = block_dev_id(bdevs[j]);

					unit = convert_unit(&size);
					printf("%4" PRIdMAX " | %6s         | %10" PRId64 "%s | %s\n",
					    (uintmax_t)id, name, size, unit, "disk");
				}
			}
		}
	}
	printf("\n\n");
}
/* lsblk implementation end */

/* ifconfig implementation */
static int ifconfig_print_long_info(struct in_device *iface) {
	struct net_device_stats *stat;
	unsigned char mac[] = "xx:xx:xx:xx:xx:xx";
	struct in_addr in;
	char s_in[INET_ADDRSTRLEN], s_in6[INET6_ADDRSTRLEN];

	stat = &iface->dev->stats;

	printf("%s\tLink encap:", &iface->dev->name[0]);
	if (iface->dev->flags & IFF_LOOPBACK) {
		printf("Local Loopback");
	} else {
		macaddr_print(mac, &iface->dev->dev_addr[0]);
		printf("Ethernet  HWaddr %s", mac);
	}

	printf("\n\t");
	in.s_addr = iface->ifa_address;
	printf("inet addr:%s", inet_ntop(AF_INET, &in, s_in, INET_ADDRSTRLEN));

	if (iface->dev->flags & IFF_BROADCAST) {
		in.s_addr = iface->ifa_broadcast;
		printf("  Bcast:%s", inet_ntop(AF_INET, &in, s_in, INET_ADDRSTRLEN));
	}
	in.s_addr = iface->ifa_mask;
	printf("  Mask:%s", inet_ntop(AF_INET, &in, s_in, INET_ADDRSTRLEN));

	printf("\n\t");
	printf("inet6 addr: %s/??", 
			inet_ntop(AF_INET6, &iface->ifa6_address, s_in6, INET6_ADDRSTRLEN)
	);
	printf("  Scope:Host");

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

	printf("\n\tcollisions:%ld",
			stat->collisions);

	printf("\n\tRX bytes:%ld (%ld MiB)  TX bytes:%ld (%ld MiB)",
			stat->rx_bytes, stat->rx_bytes / 1048576,
			stat->tx_bytes, stat->tx_bytes / 1048576);

	if (!(iface->dev->flags & IFF_LOOPBACK))
		printf("\n\tInterrupt:%d Base address:%p", iface->dev->irq, (void *)iface->dev->base_addr);

	printf("\n\n");

	return 0;
}

static void print_ifconfig_info(void) {
	struct in_device *iface;

	printf("Network: \n");
	for (iface = inetdev_get_first(); iface != NULL;
		 iface = inetdev_get_next(iface)) {
		if (!(iface->dev->flags & IFF_UP))
			continue;
		ifconfig_print_long_info(iface);
	}
}
/* ifconfig implementation end */

int main(int argc, char **argv) {

	int opt;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			print_error();
			return 0;
		}
	}

	print_cpuinfo();
	print_lsusb_information();
	print_lspci_information();
	print_lsblk_information();
	print_ifconfig_info();

	return 0;
}
