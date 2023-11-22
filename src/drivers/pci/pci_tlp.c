/**
 * @file
 * @brief TODO
 *
 * @date Sep 27, 2013
 * @author Eldar Abusalimov
 * @author Gazukin Andrey
 */
#include <endian.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_tlp.h>
#include <util/binalign.h>

//TODO: Remove it or make it use logger from util/log.h
#define NODEBUG
#ifdef DEBUG
#include <kernel/printk.h>
void print_tlp_packet(const unsigned char *data, int length) {
	int i;

	printk("TLP: (%d)\n", length);
	for (i = 0; i < length; i++) {
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
		tlp[cnt] = be32toh(tlp[cnt]);
	}
	return tlp;
}

static inline char tlp_get_fbe(uint32_t offset) {
	char fbe;

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

	return fbe;
}

static inline char tlp_get_lbe(uint16_t len, uint32_t offset) {
	char lbe;
	size_t length;
	int bits;

	length = TLP_3DW_HEADER_SIZE + ((len + (offset % 4) + 3) & 0xFFFFFFFC);

	lbe = 0;
	if (length <= 16) {
		return lbe;
	}

	bits = (offset + length) % 4;
	if (bits) {
		/* ((1111 >> 3) = 0001)
		 * 0001 << 3 = 1000
		 */
		lbe = (0xF >> bits) << (bits);
	}
	else {
		lbe = 0xF;
	}
	lbe <<= 4;

	return lbe;
}

int tlp_build_mem_wr(uint32_t *tlp, struct pci_slot_dev *dev, char bar,
    uint32_t offset, char fmt, uint32_t *buff, uint16_t len) {
	int hsize;
	char fbe, lbe; /* first and last Byte Enable */

	if (len > 0xFFF) {
		return -EINVAL;
	}

	fmt &= TLP_FMT_MSK;
	fmt |= TLP_FMT_DATA;

	hsize = tlp_fmt2size(fmt);

	tlp[0] = (fmt << TLP_FMT_OFFSET) | (TLP_TYPE_MEM << TLP_TYPE_OFFSET)
	         | (len >> 2);

	/* First BE */
	fbe = tlp_get_fbe(offset);
	/* Last BE */
	lbe = tlp_get_lbe(len, offset);

	tlp[1] = fbe | lbe;

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

	if (len > 0xFFF) {
		return -EINVAL;
	}
	len >>= 2;

	fmt &= TLP_FMT_MSK;

	hsize = tlp_fmt2size(fmt);
	fmt &= ~TLP_FMT_DATA; /* only 0 or 1 for memory read fmt */

	tlp[0] = (fmt << TLP_FMT_OFFSET) | (TLP_TYPE_MEM << TLP_TYPE_OFFSET) | len;

	if ((tlp[0] & 0x3FF) > 1) { /* XXX */
		tlp[1] = 0xff;          // BUS/DEVFN 0, all bytes are valid
	}
	else {
		tlp[1] = 0x0f; // BUS/DEVFN 0, all bytes are valid
	}

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
int tlp_build_conf0_rd(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where) {
	/* fmt = 000, type = 00100, length = 1 */
	tlp[0] = (0 << TLP_FMT_OFFSET) | (TLP_TYPE_CFG0 << TLP_TYPE_OFFSET) | 1;
	/* BUS/DEVFN 0, all bytes in 1 DW are valid */
	tlp[1] = 0xf;
	tlp[2] = (bus << TLP_BUS_OFFSET) | (dev_fn << TLP_DEV_FUN_OFFSET) | where;

	return 0;
}

int tlp_build_conf1_rd(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where) {
	/* fmt = 000, type = 00101, length = 1 */
	tlp[0] = (0 << TLP_FMT_OFFSET) | (TLP_TYPE_CFG1 << TLP_TYPE_OFFSET) | 1;
	/* BUS/DEVFN 0, all bytes in 1 DW are valid */
	tlp[1] = 0xf;
	tlp[2] = (bus << TLP_BUS_OFFSET) | (dev_fn << TLP_DEV_FUN_OFFSET) | where;

	return 0;
}

int tlp_build_conf0_wr(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where) {
	/* fmt = 010, type = 00100, length = 1 */
	tlp[0] = (TLP_FMT_DATA << TLP_FMT_OFFSET)
	         | (TLP_TYPE_CFG0 << TLP_TYPE_OFFSET) | 1;
	/* BUS/DEVFN 0, all bytes in 1 DW are valid */
	tlp[1] = 0xf;
	tlp[2] = (bus << TLP_BUS_OFFSET) | (dev_fn << TLP_DEV_FUN_OFFSET) | where;

	return 0;
}

int tlp_build_conf1_wr(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where) {
	/* fmt = 010, type = 00101, length = 1 */
	tlp[0] = (TLP_FMT_DATA << TLP_FMT_OFFSET)
	         | (TLP_TYPE_CFG1 << TLP_TYPE_OFFSET) | 1;
	/* BUS/DEVFN 0, all bytes in 1 DW are valid */
	tlp[1] = 0xf;
	tlp[2] = (bus << TLP_BUS_OFFSET) | (dev_fn << TLP_DEV_FUN_OFFSET) | where;

	return 0;
}
