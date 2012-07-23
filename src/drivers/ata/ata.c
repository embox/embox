/**
 * @file
 * @brief
 *
 * @date 16.07.2012
 * @author Andrey Gazukin
 */

#include <asm/io.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>
#include <drivers/pci.h>
#include <drivers/pci_utils.h>
#include <drivers/ata.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


unsigned char int_ata_status;    /* ATA status read by interrupt handler */
unsigned char int_bmide_status;  /* BMIDE status read by interrupt handler */
//unsigned char int_use_intr_flag = INT_DEFAULT_INTERRUPT_MODE;
struct _reg_cmd_info reg_cmd_info;
int reg_config_info[2];

struct timespec ts;
pci_dev_t *pci_dev;

/*
 * functions internal and private to this ATA drv
 */
static int scan_drive(uint32_t addr);
static unsigned char pio_inbyte(int addr);
static void pio_outbyte(int addr, unsigned char data);
static unsigned int pio_inword(int addr);
/*
static void pio_outword(int addr, unsigned int data);
static unsigned long pio_indword(int addr);
static void pio_outdword(int addr, unsigned long data);
 */

#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEV_ID_INTEL_IDE_82371SB 0x7010

//EMBOX_UNIT_INIT(ata_init);

/*
 *  detection_drive() - Check the host adapter and determine the
 *  number and type of drives attached.
 */
int detection_drive(void) {

	scan_drive(PRIMARY_COMMAND_REG_BASE_ADDR);
	scan_drive(SECONDARY_COMMAND_REG_BASE_ADDR);

	pci_dev = pci_find_dev(PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_IDE_82371SB);
	if (pci_dev != NULL) {
		if(scan_drive(pci_dev->bar[4] & PCI_BASE_ADDR_IO_MASK)) {
			return 0;
		}
	}
	return 0;
}

static void base_regaddr_set(uint32_t addr,
		uint32_t *base_cmd_addr, uint32_t *base_ctrl_addr) {

	if(addr) {
	   *base_cmd_addr = addr;
	   if(PRIMARY_COMMAND_REG_BASE_ADDR == *base_cmd_addr) {
		   *base_ctrl_addr = PRIMARY_CONTROL_REG_BASE_ADDR;
	   }
	   else if(SECONDARY_COMMAND_REG_BASE_ADDR == *base_cmd_addr) {
		   *base_ctrl_addr = SECONDARY_CONTROL_REG_BASE_ADDR;
	   }
	   else if(THIRD_COMMAND_REG_BASE_ADDR == *base_cmd_addr) {
		   *base_ctrl_addr = THIRD_CONTROL_REG_BASE_ADDR;
	   }
	   else if(FOURTH_COMMAND_REG_BASE_ADDR == *base_cmd_addr) {
		   *base_ctrl_addr = FOURTH_CONTROL_REG_BASE_ADDR;
	   }
	}
	else {
	   *base_cmd_addr = PRIMARY_COMMAND_REG_BASE_ADDR;
	   *base_ctrl_addr = PRIMARY_CONTROL_REG_BASE_ADDR;
	}
}


int scan_drive(uint32_t addr) {
   unsigned char read_reg;
   unsigned int read_data;
   unsigned char dev, num_dev;
   int count;
   uint32_t base_cmd_addr, base_ctrl_addr;

   base_regaddr_set(addr, &base_cmd_addr, &base_ctrl_addr);

   dev = CB_DH_DEV0;
   count = 0;
   num_dev = 0;

    do {
	   read_reg = pio_inbyte(base_cmd_addr + CB_STAT);
	   usleep(100);
	   if (1000 < count++) {
		   break;
	   }
   } while (read_reg & CB_STAT_BSY);

   while(1) {
	   pio_outbyte(base_cmd_addr + CB_DH, dev);
	   usleep(1);
	   if((dev | 0xA0) == (read_reg = pio_inbyte(base_cmd_addr + CB_DH))) {
	       /* see if devX ready or none*/
		   count = 0;
		   while (1) {
			   read_reg = pio_inbyte(base_cmd_addr + CB_STAT);
			   if((read_reg & CB_STAT_RDY) || (0 == read_reg)) {
				   break;
			   }

			   usleep(1);
			   if (1000 < count++) {
				   break;
			   }
		   }

		   if (0 != read_reg) {
			   num_dev++;

			   pio_outbyte(base_cmd_addr + CB_SC, 0);
			   pio_outbyte(base_cmd_addr + CB_SN, 0);
			   pio_outbyte(base_cmd_addr + CB_CL, 0);
			   pio_outbyte(base_cmd_addr + CB_CH, 0);

			   pio_outbyte(base_cmd_addr + CB_CMD, 0xEC);
			   usleep(10);
			   read_reg = pio_inbyte(base_cmd_addr + CB_STAT);
			   read_reg = pio_inbyte(base_cmd_addr + CB_ERR);

			   /* read  drive Identification */
			   for(int i =0; i< 256; i++) {
				   read_data = pio_inword(base_cmd_addr + CB_DATA);
				   printf("%c%c", (read_data & 0xFF00) >> 8, read_data & 0x00FF);
			   }
		   }
	   }

	   if (CB_DH_DEV0 == dev) {
		   dev = CB_DH_DEV1;
	   }
	   else {
		   break;
	   }
   }
   return num_dev;
}



/* These functions do basic IN/OUT of byte and word values: */

static unsigned char pio_inbyte(int addr) {
   //!!! read an 8-bit ATA register
   //return * pio_reg_addrs[ addr ];
	return in8(addr);
}

static void pio_outbyte(int addr, unsigned char data) {
    //!!! write an 8-bit ATA register
	//* pio_reg_addrs[ addr ] = data;
	out8(data, addr);
}

static unsigned int pio_inword(int addr) {
    //return * ((unsigned int *) pio_reg_addrs[ addr ]);
	return in16(addr);
}
/*
static void pio_outword(int addr, unsigned int data) {
    // * ((unsigned int *) pio_reg_addrs[ addr ]) = data;
	out16(data, addr);
}

static unsigned long pio_indword(int addr) {
    //return * ((unsigned long *) pio_reg_addrs[ addr ]);
	return in32(addr);
}

static void pio_outdword(int addr, unsigned long data) {
	// * ((unsigned long *) pio_reg_addrs[ addr ]) = data;
	out32(data, addr);
}
*/
