/**
 * @file
 * @brief TODO
 *
 * @date Sep 27, 2013
 * @author Eldar Abusalimov
 */
#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#include <util/binalign.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_tlp.h>
//#include <endian.h>

uint32_t betoh32(uint32_t v) {
	return v;
}

uint32_t htobe32(uint32_t v) {
	return v;
}
#define DEBUG
#ifdef DEBUG
#include <kernel/printk.h>
void print_tlp_packet(const unsigned char *data, int length)
{
	int i;

	printk("TLP: (%d)\n", length);
	for (i = 0; i < length; i++)
	{
		printk("%X ", data[i]);
		if (i % 16 == 15)
			printk("\n");
	}
	if (i % 16 != 15)
		printk("\n");
}
#else
void print_tlp_packet(const unsigned char *data, int length) {

}
#endif
static inline int tlp_fmt2size(char fmt) {
	if (TLP_FMT_HEADSIZE & fmt) {
		return 4;
	}
	return 3;
}

static inline int tlp_size(uint32_t *tlp) {
	if (tlp[0] & (TLP_FMT_HEADSIZE << TLP_FMT_OFFSET)) {
		return 4;
	}
	return 3;
}

static inline uint32_t *tlp_to_pci(uint32_t *tlp) {
	int size = tlp_size(tlp);
	int cnt;

	for (cnt = 0; cnt != size; ++cnt) {
		tlp[cnt] = htobe32(tlp[cnt]);
	}
	return tlp;
}

static inline uint32_t *tlp_from_pci(uint32_t *tlp) {
	int size = tlp_size(tlp);
	int cnt;

	for (cnt = 0; cnt != size; ++cnt) {
		tlp[cnt] = betoh32(tlp[cnt]);
	}
	return tlp;
}

int tlp_build_mem_wr(uint32_t *tlp, struct pci_slot_dev *dev, char bar,
		uint32_t offset, char fmt, uint32_t *buff, uint16_t len) {
	int hsize;
	char fbe, lbe; /* first and last Byte Enable */

	if (len > 0xFFF) {
		return -EINVAL;
	}
	hsize = tlp_fmt2size(fmt);
	fmt |= TLP_FMT_DATA;

	tlp[0] = (fmt << TLP_FMT_OFFSET) | (TLP_TYPE_MEM << TLP_TYPE_OFFSET) | len;

	switch (offset & 3) {
	case 1:
		fbe = 0x7;
		break;
	case 2:
		fbe = 0x3;
		break;
	case 3:
		fbe = 0x1;
		break;
	default:
		fbe = 0xF;
		break;
	}
	lbe = 0;

	tlp[1] = fbe | (lbe << 4);

	tlp[2] = PCI_BAR_BASE(dev->bar[bar]) + binalign_bound(offset, 4);

	if (hsize == 4) {
		tlp[3] = 0;
	}


	return 0;
}

int tlp_build_mem_rd(uint32_t *tlp, struct pci_slot_dev *dev, char bar,
		uint32_t offset, char fmt, uint32_t *buff, uint16_t len) {
	int hsize;
//	char fbe, lbe; /* first and last Byte Enable */
	uint32_t dw;

	if (len > 0xFFF) {
		return -EINVAL;
	}
	hsize = tlp_fmt2size(fmt);

	tlp[0] = (fmt << TLP_FMT_OFFSET) | (TLP_TYPE_MEM << TLP_TYPE_OFFSET) | len;

	if ((tlp[0] & 0x3FF) > 1) { /* XXX */
		dw = 0xff; // BUS/DEVFN 0, all bytes are valid
	}
	else {
		dw = 0x0f; // BUS/DEVFN 0, all bytes are valid
	}

	tlp[1] = dw;

	tlp[2] = PCI_BAR_BASE(dev->bar[bar]) + binalign_bound(offset, 4);

	if (hsize == 4) {
		tlp[3] = 0;
	}

	return 0;
}

int tlp_build_io_wr(char frm, uint32_t *buff, size_t data_len) {
	return 0;
}

int tlp_build_io_rd(char frm) {
	return 0;
}

int tlp_build_conf0_wr(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
		uint32_t where) {
	tlp[0] = (TLP_FMT_DATA << TLP_FMT_OFFSET) |
			(TLP_TYPE_CFG0 << TLP_TYPE_OFFSET) |
			1 /* length */;
	tlp[1] = 0xf; // BUS/DEVFN 0, all bytes are valid
	tlp[2] = (bus << 24) | (dev_fn << 16) | where;

	return 0;
}

int tlp_build_conf0_rd(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
		uint32_t where) {
	tlp[0] = (0 << TLP_FMT_OFFSET) | (TLP_TYPE_CFG0 << TLP_TYPE_OFFSET) | 1;
	return 0;
}

int tlp_build_conf1_wr(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
		uint32_t where) {
	tlp[0] = (TLP_FMT_DATA << TLP_FMT_OFFSET) |
			(TLP_TYPE_CFG1 << TLP_TYPE_OFFSET) |
			1 /* length */;
	return 0;
}

int tlp_build_conf1_rd(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
		uint32_t where) {
	return 0;
}
