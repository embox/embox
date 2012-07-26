/**
 * @file
 * @brief
 *
 * @date 16.07.2012
 * @author Andrey Gazukin
 */

#include <asm/io.h>
#include <fs/vfs.h>
#include <fs/fat.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>
#include <drivers/pci.h>
#include <drivers/pci_utils.h>
#include <drivers/ata.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <mem/misc/pool.h>

int ide_dev_quantity;

static uint8_t num_dev;
ide_ata_slot_t ide_ata_slot;

static int scan_drive(uint8_t *bus_number);
static unsigned char pio_inbyte(int addr);
static void pio_outbyte(int addr, unsigned char data);
static unsigned int pio_inword(int addr);
/*
static void pio_outword(int addr, unsigned int data);
static unsigned long pio_indword(int addr);
static void pio_outdword(int addr, unsigned long data);
 */
static dev_ide_ata_t *ide_drive_create(uint8_t *dev_number);
static void read_identification (dev_ide_ata_identif_t *identif,
								 uint32_t base_cmd_addr);
static void read_id_string(uint8_t *p_data,
		                   size_t size, uint32_t base_cmd_addr);
static int set_lba_cmd(dev_ide_ata_t *drive);
static int read_sectors_lba_cmd(dev_ide_ata_t *drive, unsigned long lba);
static int set_sector_count_cmd(dev_ide_ata_t *drive,
		                        unsigned char sect_count);
static int read_sectors_lba(char *buff, dev_ide_ata_t *drive,
						unsigned char sect_count, unsigned long lba);

#define PCI_VENDOR_ID_INTEL 0x8086
#define PCI_DEV_ID_INTEL_IDE_82371SB 0x7010

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

EMBOX_UNIT_INIT(ata_init);
/* ide ata devices pool */
POOL_DEF(ide_dev_pool, struct _dev_ide_ata, MAX_DEV_QUANTITY);

int ata_init(void) {

	ide_ata_slot.ide_bus[0].base_cmd_addr =
	ide_ata_slot.ide_bus[1].base_cmd_addr = PRIMARY_COMMAND_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[0].base_ctrl_addr =
	ide_ata_slot.ide_bus[1].base_ctrl_addr = PRIMARY_CONTROL_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[2].base_cmd_addr =
	ide_ata_slot.ide_bus[3].base_cmd_addr = SECONDARY_COMMAND_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[2].base_ctrl_addr =
	ide_ata_slot.ide_bus[3].base_ctrl_addr = SECONDARY_CONTROL_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[4].base_cmd_addr =
	ide_ata_slot.ide_bus[5].base_cmd_addr = TERTIARY_COMMAND_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[4].base_ctrl_addr =
	ide_ata_slot.ide_bus[5].base_ctrl_addr = TERTIARY_CONTROL_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[6].base_cmd_addr =
	ide_ata_slot.ide_bus[7].base_cmd_addr = QUATERNARY_COMMAND_REG_BASE_ADDR;
	ide_ata_slot.ide_bus[6].base_ctrl_addr =
	ide_ata_slot.ide_bus[7].base_ctrl_addr = QUATERNARY_CONTROL_REG_BASE_ADDR;

	num_dev = 0;
	ide_dev_quantity = MAX_DEV_QUANTITY;
	return 0;
}

/*
 *  detection_drive() - Check the host adapter and determine the
 *  number and type of drives attached.
 */
ide_ata_slot_t *detection_drive(void) {
	/*pci_dev_t *pci_dev; */
	static uint8_t bus_num;

	bus_num = num_dev = 0;
	for(int i = 0; i < MAX_DEV_QUANTITY; i += 2) {
		scan_drive(&bus_num);
	}

	/*
	pci_dev = pci_find_dev(PCI_VENDOR_ID_INTEL, PCI_DEV_ID_INTEL_IDE_82371SB);
	if (pci_dev != NULL) {
		if(scan_drive(pci_dev->bar[4] & PCI_BASE_ADDR_IO_MASK)) {
			return 0;
		}
	}
	*/
	return &ide_ata_slot;
}

static int scan_drive(uint8_t *bus_number) {
	vol_info_t *volinfo;
	mbr_t *mbr;
	lbr_t *lbr;
	char buff[1024];

   unsigned char read_reg;
   uint16_t read_data;
   unsigned char dev;
   int count;
   uint32_t base_cmd_addr, base_ctrl_addr;
   dev_ide_ata_t *dev_ide;

   base_cmd_addr = ide_ata_slot.ide_bus[*bus_number].base_cmd_addr;
   base_ctrl_addr = ide_ata_slot.ide_bus[*bus_number].base_ctrl_addr;

   dev = CB_DH_DEV0;
   count = 0;

   do {
	   read_reg = pio_inbyte(base_cmd_addr + CB_STAT);
	   usleep(100);
	   if (1000 < count++) {
		   return 0;
	   }
   } while(read_reg & CB_STAT_BSY);

   while(1) {
	   ide_ata_slot.ide_bus[*bus_number].dev_ide_ata = NULL;
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

		   if ((0 != read_reg) &&
			  (NULL != (dev_ide = ide_drive_create(&num_dev)))) {

			   ide_ata_slot.ide_bus[*bus_number].dev_ide_ata = dev_ide;

			   pio_outbyte(base_cmd_addr + CB_SC, 0);
			   pio_outbyte(base_cmd_addr + CB_SN, 0);
			   pio_outbyte(base_cmd_addr + CB_CL, 0);
			   pio_outbyte(base_cmd_addr + CB_CH, 0);

			   pio_outbyte(base_cmd_addr + CB_CMD, 0xEC);
			   usleep(10);
			   read_reg = pio_inbyte(base_cmd_addr + CB_STAT);
			   read_reg = pio_inbyte(base_cmd_addr + CB_ERR);

			   /* read  drive Identification */
			   read_identification(&dev_ide->identification, base_cmd_addr);

			   for(int i =160; i< 256; i++) {
				   read_data = pio_inword(base_cmd_addr + CB_DATA);
			   }
			   dev_ide->base_cmd_addr = base_cmd_addr;
			   dev_ide->base_ctrl_addr = base_ctrl_addr;
			   //dev_ide->irq = irq;
			   dev_ide->master_flg = dev;
			   set_lba_cmd(dev_ide);
			   for (int i=0; i<500; i++){
				   read_sectors_lba(buff, dev_ide, 1, i);
				   volinfo = (vol_info_t *)buff;
				   if(volinfo->filesystem == FAT32){
					   printf("!!!");
				   }
				   mbr = (mbr_t *)buff;
				   if((mbr->sig_55 == 0x55) || (mbr->sig_55 != 0)) {
					   printf("\nqwe");
				   }

				   lbr = (lbr_t *)buff;
				   if((lbr->sig_55 == 0x55)|| (lbr->sig_55 != 0)) {
					   printf("\newry");
				   }
			   }
		   }
	   }
	   (*bus_number)++;
	   if(CB_DH_DEV0 == dev) {
		   dev = CB_DH_DEV1;
		   base_cmd_addr = ide_ata_slot.ide_bus[*bus_number].base_cmd_addr;
		   base_ctrl_addr = ide_ata_slot.ide_bus[*bus_number].base_ctrl_addr;
	   }
	   else {
		   break;
	   }
   }
   return num_dev;
}

static void read_identification (dev_ide_ata_identif_t *identif,
										uint32_t base_cmd_addr) {
	uint16_t *p_data16;

	identif->config = pio_inword(base_cmd_addr + CB_DATA);
	identif->num_cyl = pio_inword(base_cmd_addr + CB_DATA);
	identif->rsrv1 = pio_inword(base_cmd_addr + CB_DATA);
	identif->num_head = pio_inword(base_cmd_addr + CB_DATA);
	identif->bytes_pr_track = pio_inword(base_cmd_addr + CB_DATA);
	identif->bytes_pr_sect = pio_inword(base_cmd_addr + CB_DATA);
	identif->sect_pr_track = pio_inword(base_cmd_addr + CB_DATA);

	read_id_string((uint8_t *)identif->vendor,
					       sizeof(identif->vendor)/2, base_cmd_addr);

	read_id_string((uint8_t *)identif->sn,
				       sizeof(identif->sn)/2, base_cmd_addr);

	identif->buff_type = pio_inword(base_cmd_addr + CB_DATA);
	identif->buff_size = pio_inword(base_cmd_addr + CB_DATA);
	identif->num_ecc = pio_inword(base_cmd_addr + CB_DATA);

	read_id_string((uint8_t *)identif->fw_rev,
			       sizeof(identif->fw_rev)/2, base_cmd_addr);

	read_id_string((uint8_t *)identif->model_numb,
			       sizeof(identif->model_numb)/2, base_cmd_addr);

	identif->numb_transfer = pio_inword(base_cmd_addr + CB_DATA);
	identif->dbl_word_flg = pio_inword(base_cmd_addr + CB_DATA);
	identif->capabilities = pio_inword(base_cmd_addr + CB_DATA);
	identif->rsrv2 = pio_inword(base_cmd_addr + CB_DATA);
	identif->pio_timing_mode = pio_inword(base_cmd_addr + CB_DATA);
	identif->dma_timing_mode = pio_inword(base_cmd_addr + CB_DATA);
	identif->valid = pio_inword(base_cmd_addr + CB_DATA);
	identif->cur_num_cyl = pio_inword(base_cmd_addr + CB_DATA);
	identif->cur_num_head = pio_inword(base_cmd_addr + CB_DATA);
	identif->cur_sct_pr_track = pio_inword(base_cmd_addr + CB_DATA);

	p_data16 = (uint16_t *)&identif->capacity;
	*p_data16 = pio_inword(base_cmd_addr + CB_DATA);
	p_data16++;
	*p_data16 = pio_inword(base_cmd_addr + CB_DATA);

	identif->num_in_multiple = pio_inword(base_cmd_addr + CB_DATA);

	p_data16 = (uint16_t *)&identif->num_user_sect;
	*p_data16 = pio_inword(base_cmd_addr + CB_DATA);
	p_data16++;
	*p_data16 = pio_inword(base_cmd_addr + CB_DATA);

	identif->dma_mode = pio_inword(base_cmd_addr + CB_DATA);
	identif->multiword_dma = pio_inword(base_cmd_addr + CB_DATA);

	read_id_string((uint8_t *)identif->rsrv3,
				       sizeof(identif->rsrv3)/2, base_cmd_addr);

	read_id_string((uint8_t *)identif->vendor_uniq,
				       sizeof(identif->vendor_uniq)/2, base_cmd_addr);
}

static void read_id_string(uint8_t *p_data,
		                    size_t size, uint32_t base_cmd_addr) {
	uint16_t data;

	for(int i =0; i < size; i++) {
		data = pio_inword(base_cmd_addr + CB_DATA);
		*p_data++ = (data & 0xFF00) >> 8;
		*p_data++ = data & 0x00FF;
	}
	/*set string null determine */
	*p_data = 0;
}

static dev_ide_ata_t *ide_drive_create(uint8_t *dev_number) {
	dev_ide_ata_t *dev_ide;
	node_t *dev_node;
	char dev_path[MAX_LENGTH_PATH_NAME];
	char dev_name[MAX_LENGTH_FILE_NAME];

	if (MAX_DEV_QUANTITY <= *dev_number) {
		return NULL;
	}

	*dev_path = 0;
	strcat(dev_path, "/dev/");
	dev_name[0] = 'h';
	dev_name[1] = 'd';
	dev_name[2] = 'a' + *dev_number;
	dev_name[3] = 0;
	strcat(dev_path, dev_name);

	if (NULL == (dev_node = vfs_add_path(dev_path, NULL))) {
		if (NULL == (dev_node = vfs_find_node(dev_path, NULL))) {
			return NULL;
		}
		else {
			(*dev_number)++;
			return (dev_ide_ata_t *) dev_node->attr;/*dev already exist*/
		}
	}

	if(NULL == (dev_ide = pool_alloc(&ide_dev_pool))) {
		return dev_ide;
	}
	(*dev_number)++;
	strcpy(dev_ide->dev_name, dev_name);
	dev_ide->dev_node = dev_node;
	dev_ide->dev_node->attr = (void *) dev_ide;
	return dev_ide;
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

static int set_lba_cmd(dev_ide_ata_t *drive) {
	unsigned char data;

	data = drive->master_flg | CB_LBA_MODE;

	if(drive->identification.capabilities & LBA_SUPP) {
		pio_outbyte(drive->base_cmd_addr + CB_DH, data);
		return 0;
	}
	else {
		return -1;
	}
}

static int set_sector_count_cmd(dev_ide_ata_t *drive, unsigned char sect_count) {

    pio_outbyte(drive->base_cmd_addr + CB_SC, sect_count);
    return 0;
}

static  int read_sectors_lba_cmd(dev_ide_ata_t *drive, unsigned long lba) {

    pio_outbyte(drive->base_cmd_addr + CB_LBA_000L, lba & 0xFF);
    pio_outbyte(drive->base_cmd_addr + CB_LBA_00H0, (lba >> 8) & 0xFF);
    pio_outbyte(drive->base_cmd_addr + CB_LBA_0L00, (lba >> 16) & 0xFF);
    pio_outbyte(drive->base_cmd_addr + CB_LBA_H000, (lba >> 24) & 0xFF);

    pio_outbyte(drive->base_cmd_addr + CB_CMD, CMD_READ_SECTORS);
    usleep(10);
	return 0;
}

static int read_sectors_lba(char *buff, dev_ide_ata_t *drive,
		unsigned char sect_count, unsigned long lba) {
	char *p_data;
	uint16_t reg;
	unsigned char count;

	p_data = buff;
	count = sect_count;
	set_sector_count_cmd(drive, sect_count);
	read_sectors_lba_cmd(drive, lba);
	do {

		for(int i =0; i < (drive->identification.bytes_pr_sect / 2); i++) {
			reg = pio_inword(drive->base_cmd_addr + CB_DATA);
			*p_data++ = reg & 0xFF;
			*p_data++ = (reg >> 8) & 0xFF;
		}

	} while (--count);

	return 0;
}

/*
static int format_drive_cmd(dev_ide_ata_t *drive) {

	return 0;
}

static int write_sector_lba_cmd(dev_ide_ata_t *drive, unsigned long lba) {

	return 0;
}

static int seek_cmd(dev_ide_ata_t *drive) {

	return 0;
}
*/
