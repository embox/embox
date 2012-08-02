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
#include <embox/block_dev.h>

int ide_dev_quantity;

static uint8_t num_dev;
ide_ata_slot_t ide_ata_slot;

static unsigned char int_use_intr_flag = INT_DEFAULT_INTERRUPT_MODE;
static long tmr_cmd_start_time;      // command start time


static int scan_drive(uint8_t *bus_number);

static unsigned long pio_set_base(dev_ide_ata_t *drive, char cmd);
static unsigned char pio_inbyte(dev_ide_ata_t *drive, char cmd, int addr);
static void pio_outbyte(dev_ide_ata_t *drive, char cmd,
								int addr, unsigned char data);
static unsigned int pio_inword(dev_ide_ata_t *drive, char cmd, int addr);
static void pio_outword(dev_ide_ata_t *drive, char cmd,
							int addr, unsigned int data);
/*
static unsigned long pio_indword(dev_ide_ata_t *drive, char cmd, int addr);
static void pio_outdword(dev_ide_ata_t *drive, char cmd,
							int addr, unsigned long data);
 */
static dev_ide_ata_t *ide_drive_create(uint8_t *dev_number);
static void read_identification (dev_ide_ata_identif_t *identif,
								 int base_cmd_addr);
static void read_id_string(uint8_t *p_data,
		                   size_t size, int base_cmd_addr);
static int enable_lba_cmd(dev_ide_ata_t *drive);
static int disable_lba_cmd(dev_ide_ata_t *drive);
static int read_sectors_lba_cmd(dev_ide_ata_t *drive, unsigned long lba);
static int write_sectors_lba_cmd(dev_ide_ata_t *drive, unsigned long lba);
static int set_sector_count_cmd(dev_ide_ata_t *drive,
		                        unsigned char sect_count);
static size_t read_sectors_lba(void *dev, char *buff,
								uint32_t lba, uint32_t sect_count);
static size_t write_sectors_lba(void *dev, char *buff,
								uint32_t lba, uint32_t sect_count);
static int sub_wait_poll(int addr);
static void tmr_set_timeout(void);
static int tmr_chk_timeout(void);
static long system_read_timer(void);
static int system_wait_intr_or_timeout(void);
static int ioctl(void *dev);
static int flush(void *dev);

static block_dev_operations_t block_dev_op = {
		.blk_read = read_sectors_lba,
		.blk_write = write_sectors_lba,
		.ioctl = ioctl,
		.flush = flush
};

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
   unsigned char dev;
   int count;
   int base_cmd_addr, base_ctrl_addr;
   dev_ide_ata_t *dev_ide;

   base_cmd_addr = ide_ata_slot.ide_bus[*bus_number].base_cmd_addr;
   base_ctrl_addr = ide_ata_slot.ide_bus[*bus_number].base_ctrl_addr;

   dev = CB_DH_DEV0;
   count = 0;

   do {
	   read_reg = pio_inbyte(NULL, COMMAND, base_cmd_addr + CB_STAT);
	   usleep(100);
	   if (1000 < count++) {
		   return 0;
	   }
   } while(read_reg & CB_STAT_BSY);

   while(1) {
	   tmr_set_timeout();
	   ide_ata_slot.ide_bus[*bus_number].dev_ide_ata = NULL;
	   pio_outbyte(NULL, COMMAND, base_cmd_addr + CB_DH, dev);

	   if((dev | 0xA0) ==
			   (read_reg = pio_inbyte(NULL, COMMAND, base_cmd_addr + CB_DH))) {
	       /* see if devX ready or none*/
		   count = 0;
		   while (1) {
			   read_reg = pio_inbyte(NULL, COMMAND, base_cmd_addr + CB_STAT);
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

			   pio_outbyte(NULL, COMMAND, base_cmd_addr + CB_SC, 0);
			   pio_outbyte(NULL, COMMAND, base_cmd_addr + CB_SN, 0);
			   pio_outbyte(NULL, COMMAND, base_cmd_addr + CB_CL, 0);
			   pio_outbyte(NULL, COMMAND, base_cmd_addr + CB_CH, 0);

			   pio_outbyte(NULL, COMMAND, base_cmd_addr + CB_CMD, 0xEC);
			   usleep(10);
			   read_reg = pio_inbyte(NULL, COMMAND, base_cmd_addr + CB_STAT);
			   read_reg = pio_inbyte(NULL, COMMAND, base_cmd_addr + CB_ERR);

			   /* read  drive Identification */
			   read_identification(&dev_ide->identification, base_cmd_addr);

			   dev_ide->base_cmd_addr = base_cmd_addr;
			   dev_ide->base_ctrl_addr = base_ctrl_addr;
			   //dev_ide->irq = irq;
			   dev_ide->master_select = dev;
			   for (int i=0; i<50; i++){
				   read_sectors_lba(dev_ide, buff, i, 1);
				   volinfo = (vol_info_t *)buff;
				   if(volinfo->filesystem == FAT16){
					   printf("impossible");
				   }
				   mbr = (mbr_t *)buff;
				   if((mbr->sig_55 == 0x55) && (mbr->sig_aa == 0xAA)) {
					   printf("\n");
				   }

				   lbr = (lbr_t *)buff;
				   if((lbr->sig_55 == 0x55) && (mbr->sig_aa == 0xAA)) {
					   printf("%s", lbr->ebpb.ebpb.label);
				   }
				   write_sectors_lba(dev_ide, buff, i, 1);
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
										int base_cmd_addr) {
	uint16_t *p_data16;

	identif->config = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->num_cyl = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->rsrv1 = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->num_head = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->bytes_pr_track =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->bytes_pr_sect =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->sect_pr_track =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	read_id_string((uint8_t *)identif->vendor,
					       sizeof(identif->vendor)/2, base_cmd_addr);

	read_id_string((uint8_t *)identif->sn,
				       sizeof(identif->sn)/2, base_cmd_addr);

	identif->buff_type = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->buff_size = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->num_ecc = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	read_id_string((uint8_t *)identif->fw_rev,
			       sizeof(identif->fw_rev)/2, base_cmd_addr);

	read_id_string((uint8_t *)identif->model_numb,
			       sizeof(identif->model_numb)/2, base_cmd_addr);

	identif->numb_transfer =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->dbl_word_flg = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->capabilities = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->rsrv2 = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->pio_timing_mode =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->dma_timing_mode =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->valid = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->cur_num_cyl = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->cur_num_head = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->cur_sct_pr_track =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	p_data16 = (uint16_t *)&identif->capacity;
	*p_data16 = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	p_data16++;
	*p_data16 = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	identif->num_in_multiple =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	p_data16 = (uint16_t *)&identif->num_user_sect;
	*p_data16 = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	p_data16++;
	*p_data16 = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	identif->dma_mode = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
	identif->multiword_dma =
			pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);

	read_id_string((uint8_t *)identif->rsrv3,
				       sizeof(identif->rsrv3)/2, base_cmd_addr);

	read_id_string((uint8_t *)identif->vendor_uniq,
				       sizeof(identif->vendor_uniq)/2, base_cmd_addr);

	for(int i =160; i< 256; i++) {
	   pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
   }
}

static void read_id_string(uint8_t *p_data,
		                    size_t size, int base_cmd_addr) {
	uint16_t data;

	for(int i =0; i < size; i++) {
		data = pio_inword(NULL, COMMAND, base_cmd_addr + CB_DATA);
		*p_data++ = (data & 0xFF00) >> 8;
		*p_data++ = data & 0x00FF;
	}
	/*set string null determine */
	*p_data = 0;
}

static dev_ide_ata_t *ide_drive_create(uint8_t *dev_number) {
	dev_ide_ata_t *dev_ide;
	node_t *dev_node;
	block_dev_module_t *block_dev;
	char dev_path[MAX_LENGTH_PATH_NAME];
	char dev_name[MAX_LENGTH_FILE_NAME];
	double size;

	if (MAX_DEV_QUANTITY <= *dev_number) {
		return NULL;
	}

	*dev_path = 0;
	strcat(dev_path, "/dev/");
	dev_name[0] = 's';
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
	//strcpy(dev_ide->dev_name, dev_name);
	dev_ide->dev_node = dev_node;

	if(NULL == (block_dev = block_dev_find("harddisk"))) {
		return NULL;
	}
	dev_node->file_info = (void *) block_dev->dev_ops;

	dev_ide->dev_node->attr = (void *) dev_ide;
	size =
		(double) dev_ide->identification.num_cyl *
		(double) dev_ide->identification.num_head *
		(double) dev_ide->identification.bytes_pr_sect *
		(double) (dev_ide->identification.sect_pr_track + 1) / 1024 / 1024;
		dev_ide->size = (size_t)size;
	return dev_ide;
}

static unsigned long pio_set_base(dev_ide_ata_t *drive, char cmd) {
	if(NULL == drive) {
		/* if drive not set, base can be set in addr */
		if(cmd) {
			return 0;
		}
		else {
			return 0;
		}
	}
	else {
		if(cmd) {
			return (unsigned long) drive->base_cmd_addr;
		}
		else {
			return (unsigned long) drive->base_ctrl_addr;
		}
	}
}
/* These functions do basic IN/OUT of byte and word values: */
static unsigned char pio_inbyte(dev_ide_ata_t *drive, char cmd, int addr) {
	unsigned long base;
	base = pio_set_base(drive, cmd);
    /*!!! read an 8-bit ATA register */
	return in8(base + (unsigned long) addr);
}

static void pio_outbyte(dev_ide_ata_t *drive, char cmd,
		int addr, unsigned char data) {
	unsigned long base;
	base = pio_set_base(drive, cmd);
    /*!!! write an 8-bit ATA register*/
	out8(data, base + (unsigned long) addr);
}

static unsigned int pio_inword(dev_ide_ata_t *drive, char cmd, int addr) {
	unsigned long base;
	base = pio_set_base(drive, cmd);
	return in16(base + (unsigned long) addr);
}

static void pio_outword(dev_ide_ata_t *drive, char cmd,
		int addr, unsigned int data) {
unsigned long base;
	base = pio_set_base(drive, cmd);
	out16(data, base + (unsigned long) addr);
}
/*
static unsigned long pio_indword(dev_ide_ata_t *drive, char cmd, int addr) {
	unsigned long base;
	if(NULL == drive) {
		base = PRIMARY_COMMAND_REG_BASE_ADDR;
	}
	else {
		if(cmd) {
			base = drive->base_cmd_addr;
		}
		else {
			base = drive->base_ctrl_addr;
		}
	}
	return in32(base + (unsigned long) addr);
}

static void pio_outdword(dev_ide_ata_t *drive, char cmd,
		int addr, unsigned long data) {
	unsigned long base;
	base = pio_set_base(drive, cmd);
	out32(data, base + (unsigned long) addr);
}
*/

static int enable_lba_cmd(dev_ide_ata_t *drive) {
	unsigned char data;

	if(NULL == drive) {
			return -1;
		}
	data = drive->master_select | CB_LBA_MODE;

	if(drive->identification.capabilities & LBA_SUPP) {
		if(sub_wait_poll(drive->base_ctrl_addr)) {
			return -1;
		}

		pio_outbyte(drive, COMMAND, CB_DH, data);
		return 0;
	}
	else {
		return -1;
	}
}

static int disable_lba_cmd(dev_ide_ata_t *drive) {
	unsigned char data;

	data = drive->master_select & ~CB_LBA_MODE;

	if(drive->identification.capabilities & LBA_SUPP) {
		if(sub_wait_poll(drive->base_ctrl_addr)) {
			return -1;
		}

		pio_outbyte(drive, COMMAND, CB_DH, data);
		return 0;
	}
	else {
		return -1;
	}
}

static int set_sector_count_cmd(dev_ide_ata_t *drive, unsigned char sect_count) {

    pio_outbyte(drive, COMMAND, CB_SC, sect_count);
    return 0;
}


static  int set_lba_addr(dev_ide_ata_t *drive, unsigned long lba) {
	uint8_t data_reg;

	pio_outbyte(drive, COMMAND, CB_LBA_000L, lba & 0xFF);
    pio_outbyte(drive, COMMAND, CB_LBA_00H0, (lba >> 8) & 0xFF);
    pio_outbyte(drive, COMMAND, CB_LBA_0L00, (lba >> 16) & 0xFF);
    data_reg = drive->master_select;
    data_reg |= ((lba >> 24) & 0x0F) | CB_LBA_MODE;
    pio_outbyte(drive, COMMAND, CB_LBA_H000, data_reg);

	return 0;
}

static  int read_sectors_lba_cmd(dev_ide_ata_t *drive, unsigned long lba) {

	set_lba_addr(drive, lba);

    pio_outbyte(drive, COMMAND, CB_CMD, CMD_READ_SECTORS);
    usleep(10);
	return 0;
}

static  int write_sectors_lba_cmd(dev_ide_ata_t *drive, unsigned long lba) {

	set_lba_addr(drive, lba);

    pio_outbyte(drive, COMMAND, CB_CMD, CMD_WRITE_SECTORS);
    usleep(10);
	return 0;
}

static size_t read_sectors_lba(void *dev, char *buff,
	    					uint32_t lba, uint32_t sect_count) {
	char *p_data;
	uint16_t reg;
	unsigned char count;
	size_t rezult;
	dev_ide_ata_t *drive;

	drive = (dev_ide_ata_t *) dev;

	p_data = buff;
	count = sect_count;
	tmr_set_timeout();

	if(sub_wait_poll(drive->base_ctrl_addr)) {
		return -1;
	}
	if(0 > enable_lba_cmd(drive)) {
		return -1;
	}
	set_sector_count_cmd(drive, sect_count);
	read_sectors_lba_cmd(drive, lba);
	rezult = 0;
	do {
		if(sub_wait_poll(drive->base_ctrl_addr)) {
			rezult = -1;
			break;
		}
		for(int i =0; i < (drive->identification.bytes_pr_sect / 2); i++) {
			reg = pio_inword(drive, COMMAND, CB_DATA);
			*p_data++ = reg & 0xFF;
			*p_data++ = (reg >> 8) & 0xFF;
		}
		tmr_set_timeout();
	} while (--count);
	disable_lba_cmd(drive);

	return rezult;
}

static size_t write_sectors_lba(void *dev, char *buff,
	    					 uint32_t lba, uint32_t sect_count) {
	uint16_t *p_data;
	unsigned char count;
	size_t rezult;
	dev_ide_ata_t *drive;

	drive = (dev_ide_ata_t *) dev;

	p_data = (uint16_t *) buff;
	count = sect_count;
	tmr_set_timeout();

	if(sub_wait_poll(drive->base_ctrl_addr)) {
		return -1;
	}
	enable_lba_cmd(drive);
	set_sector_count_cmd(drive, sect_count);
	write_sectors_lba_cmd(drive, lba);
	rezult = 0;
	do {
		if(sub_wait_poll(drive->base_ctrl_addr)) {
			rezult = -1;
			break;
		}
		for(int i =0; i < (drive->identification.bytes_pr_sect / 2); i++) {
			pio_outword(drive, COMMAND, CB_DATA, *p_data++);
		}
		tmr_set_timeout();
	} while (--count);
	disable_lba_cmd(drive);

	return rezult;
}

static int ioctl(void *dev) {

	return 0;
}
static int flush(void *dev) {

	return 0;
}

/*
static int format_drive_cmd(dev_ide_ata_t *drive) {

	return 0;
}

static int seek_cmd(dev_ide_ata_t *drive) {

	return 0;
}
*/

static int sub_wait_poll(int addr) {
   unsigned char status;

   /* Wait for interrupt -or- wait for not BUSY -or- wait for time out. */
   if (int_use_intr_flag) {
      return system_wait_intr_or_timeout();    /* time out ? */
   }
   else {
      while (1) { /* poll for not busy */
         status = pio_inbyte(NULL, CONTROL, addr + CB_ASTAT);
         if ((status & CB_STAT_BSY) == 0) {
            return 0;
         }
         if (tmr_chk_timeout()) {  /* time out yet ? */
            return 1;
         }
      }
   }
}

static void tmr_set_timeout(void) {
   /* get the command start time */
   tmr_cmd_start_time = system_read_timer();
}

static int tmr_chk_timeout(void) {
   long curTime;

   /* get current time */
   curTime = system_read_timer();

   /* timed out yet ? */
   if (curTime >= (tmr_cmd_start_time + TMR_TIME_OUT)) {
      return 1;      /* yes */
   }
   /* no timeout yet */
   return 0;
}

static long system_read_timer(void) {
	struct timespec ts;
	ktime_get_timespec (&ts);
    return (long) ts.tv_sec;
}

static int system_wait_intr_or_timeout(void) {
	usleep(100);
    return 0;
}

EMBOX_BLOCK_DEV("harddisk", &block_dev_op);
