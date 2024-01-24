/**
 * @file
 *
 * @date Feb 9, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_KERNEL_IRQ_MSI_IRQ_MSI_H_
#define SRC_KERNEL_IRQ_MSI_IRQ_MSI_H_

#include <stdint.h>

#include <lib/libds/dlist.h>

/* Dummy shadow structures if an architecture does not define them */
#ifndef arch_msi_msg_addr_lo
typedef struct arch_msi_msg_addr_lo {
	uint32_t address_lo;
} __attribute__ ((packed)) arch_msi_msg_addr_lo_t;
#endif

#ifndef arch_msi_msg_addr_hi
typedef struct arch_msi_msg_addr_hi {
	uint32_t address_hi;
} __attribute__ ((packed)) arch_msi_msg_addr_hi_t;
#endif

#ifndef arch_msi_msg_data
typedef struct arch_msi_msg_data {
	uint32_t data;
} __attribute__ ((packed)) arch_msi_msg_data_t;
#endif

struct msi_msg {
	union {
		uint32_t               address_lo;
		arch_msi_msg_addr_lo_t arch_addr_lo;
	};
	union {
		uint32_t               address_hi;
		arch_msi_msg_addr_hi_t arch_addr_hi;
	};
	union {
		uint32_t               data;
		arch_msi_msg_data_t    arch_data;
	};
};

struct msi_desc {
	/* Shared device/bus type independent data */
	struct dlist_head  list;
	unsigned int       irq;
	unsigned int       nvec_used;
	void              *dev;
	struct msi_msg     msg;

	void (*write_msi_msg)(struct msi_desc *entry, void *data);
	void *write_msi_msg_data;

	union {
		/* PCI MSI/X specific data */
		struct {
			uint32_t masked;
			struct {
				uint8_t is_msix     : 1;
				uint8_t multiple    : 3;
				uint8_t multi_cap   : 3;
				uint8_t maskbit     : 1;
				uint8_t is_64       : 1;
				uint8_t is_virtual  : 1;
				uint16_t entry_nr;
				unsigned default_irq;
			} msi_attrib;
			union {
				uint8_t  mask_pos;
				void    *mask_base;
			};
		};

	};
};

extern struct msi_desc *msi_entry_alloc(void *dev, int nvec);

extern void msi_entry_entry(struct msi_desc *entry);


#endif /* SRC_KERNEL_IRQ_MSI_IRQ_MSI_H_ */
