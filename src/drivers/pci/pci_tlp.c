/**
 * @file
 * @brief TODO
 *
 * @date Sep 27, 2013
 * @author Eldar Abusalimov
 */
#include <stdint.h>
#include <stddef.h>
#include <drivers/pci/pci_tlp.h>
#include <endian.h>

static inline int tlp_fmt2size(char fmt) {
	if (TLP_FMT_HEADSIZE & fmt) {
		return 4;
	}
	return 3;
}

#define TLP_FMT_OFFSET  28
#define TLP_TYPE_OFFSET 24
int tlp_build_mem_wr(uint32_t *tlp, char frm, uint32_t *buff, size_t data_len) {
	int hsize;


	hsize = tlp_fmt2size(fmt);

	tlp[0] = (frm << TLP_FMT_OFFSET) | (TLP_TYPE_MRD << TLP_TYPE_OFFSET) |
			data_len;
	switch(offset & 3) {
	case
	}
	tlp[1] =

	htobe32()
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
