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

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_tlp.h>
//#include <endian.h>

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
		//tlp[cnt] = htobe32(tlp[cnt]);
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

	tlp[1] = fbe | lbe << 4;

	tlp[2] = dev->bar[bar];

	if (hsize == 4) {
		tlp[3] = 0;
	}


	return 0;
}

int tlp_build_mem_rd(char frm) {
	return 0;
}

int tlp_build_io_wr(char frm, uint32_t *buff, size_t data_len) {
	return 0;
}

int tlp_build_io_rd(char frm) {
	return 0;
}

int tlp_build_conf_wr(char type, char *buff, size_t data_len) {
	return 0;
}

int tlp_build_conf_rd(char type) {
	return 0;
}
