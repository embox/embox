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
#include <unistd.h>


unsigned char int_ata_status;    /* ATA status read by interrupt handler */
unsigned char int_bmide_status;  /* BMIDE status read by interrupt handler */
unsigned char int_use_intr_flag = INT_DEFAULT_INTERRUPT_MODE;
struct _reg_cmd_info reg_cmd_info;
int reg_config_info[2];
unsigned char * pio_bmide_base_addr = 0;//PIO_BMIDE_BASE_ADDR;

struct timespec ts;
pci_dev_t *pci_dev;

int count;
/*
 * functions internal and private to this ATA drv
 */

static unsigned char pio_inbyte(unsigned char addr);
static void pio_outbyte(int addr, unsigned char data);
static unsigned int pio_inword(unsigned char addr);
/*
static void pio_outword(int addr, unsigned int data);
static unsigned long pio_indword(unsigned char addr);
static void pio_outdword(int addr, unsigned long data);
 */

#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEV_ID_INTEL_IDE_82371SB 0x7010

static uint32_t base_addr;

EMBOX_UNIT_INIT(ata_init);

static int ata_init(void) {
	pci_dev = pci_find_dev(PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_IDE_82371SB);
	if (pci_dev == NULL) {
		//LOG_WARN("Couldn't find Intel 82371SB (PIIX3) IDE PCI device\n");
		//return -ENODEV;
		return -1;
	}

	base_addr = pci_dev->bar[4] & PCI_BASE_ADDR_IO_MASK;
	reg_config();
	return 0;
}

/*
 *  reg_config() - Check the host adapter and determine the
 *  number and type of drives attached.
 *
 *  This process is not documented by any of the ATA standards.
 *
 *  Infomation is returned by this function is in
 *  reg_config_info[] -- see ata.h
 *
 */
int reg_config(void) {
   unsigned char read_reg;
   unsigned int read_data;
   unsigned char dev;


   dev = CB_DH_DEV0;
   while(1) {
	   while (1) {
		   read_reg = pio_inbyte(CB_STAT);
		   if(!(read_reg & CB_STAT_BSY)) {
			   break;
		   }
	   }

	   pio_outbyte(CB_DH, dev);
	   usleep(1);
	   /* see if dev0 used */
	   read_reg = pio_inbyte(CB_DH);
	   if (CB_DH_DS0 != (CB_DH_DS0 & read_reg)) {
		   read_reg = pio_inbyte(CB_ERR);
	   }
	   else {

		   /* see if devX ready*/
		   count = 0;
		   while (1) {
			   read_reg = pio_inbyte(CB_STAT);
			   if(read_reg & CB_STAT_RDY) {
				   break;
			   }
			   usleep(1);
			   if (1000 < count++) {
				   break;
			   }
		   }

		   pio_outbyte(CB_CMD, 0xEC);
		   sleep(10);
		   read_reg = pio_inbyte(CB_STAT);

		   /* read  drive Identification */
		   read_data = pio_inword(CB_DATA);
	   }

	   if (CB_DH_DEV0 == dev) {
		   dev = CB_DH_DEV1;
	   }
	   else {
		   break;
	   }
   }
   return 0;
}



/* These functions do basic IN/OUT of byte and word values: */

static unsigned char pio_inbyte(unsigned char addr) {
   //!!! read an 8-bit ATA register
   //return * pio_reg_addrs[ addr ];
	return in8(base_addr + addr);
}

static void pio_outbyte(int addr, unsigned char data) {
    //!!! write an 8-bit ATA register
	//* pio_reg_addrs[ addr ] = data;
	out8(data, base_addr + addr);
}

static unsigned int pio_inword(unsigned char addr) {
    //return * ((unsigned int *) pio_reg_addrs[ addr ]);
	return in16(base_addr + addr);
}
/*
static void pio_outword(int addr, unsigned int data) {
    // * ((unsigned int *) pio_reg_addrs[ addr ]) = data;
	out16(data, base_addr + addr);
}

static unsigned long pio_indword(unsigned char addr) {
    //return * ((unsigned long *) pio_reg_addrs[ addr ]);
	return in32(base_addr + addr);
}

static void pio_outdword(int addr, unsigned long data) {
	// * ((unsigned long *) pio_reg_addrs[ addr ]) = data;
	out32(data, base_addr + addr);
}
*/
