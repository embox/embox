/**
 * @file
 * @brief
 *
 * @date 16.07.2012
 * @author Andrey Gazukin
 */

#ifndef ATA_H_
#define ATA_H_

#include <fs/node.h>

#define INCLUDE_ATA_DMA   0   // not zero to include ATA_DMA
#define INCLUDE_ATAPI_PIO 0   // not zero to include ATAPI PIO
#define INCLUDE_ATAPI_DMA 0   // not zero to include ATAPI DMA

extern int ide_dev_quantity;
#define INT_DEFAULT_INTERRUPT_MODE 0


typedef struct _dev_ide_ata_identif
{
	uint16_t config;           /*General configuration bit-significant information*/
	uint16_t num_cyl;          /*Number of cylinders */
	uint16_t rsrv1;            /*Reserved */
	uint16_t num_head;         /* Number of heads */
	uint16_t bytes_pr_track;   /*Number of unformatted bytes per track */
	uint16_t bytes_pr_sect;    /*Number of unformatted bytes per sector */
	uint16_t sect_pr_track;    /* Number of sectors per track */
	uint8_t  vendor[6 + 1];     /* Vendor unique */
	uint8_t  sn[20 + 1];       /* Serial number (20 ASCII characters, 0000h=not specified)*/
	uint16_t buff_type;        /* Buffer type */
	uint16_t buff_size;        /* Buffer size in 512 byte increments (0000h=not specified) */
	uint16_t num_ecc;          /* # of ECC bytes avail on read/write long cmds (0000h=not spec'd) | */
	uint8_t  fw_rev[8 + 1];    /* Firmware revision (8 ASCII characters, 0000h=not specified) */
	uint8_t  model_numb[40 + 1];   /* Model number (40 ASCII characters, 0000h=not specified) */
	uint16_t numb_transfer;    /* 15-8 Vendor unique
	 	 	 	 	 	 	    *  7-0 00h = Read/write multiple commands not implemented
	 	 	 	 	 	 	    *      xxh = Maximum number of sectors that can be transferred
	 	 	 	 	 	 	    *     	per interrupt on read and write multiple commands
	 	 	 	 	 	 	    */
	uint16_t dbl_word_flg;     /* 0000h = cannot perform doubleword I/O	Included for backwards
							    * 0001h = can perform doubleword I/O
							    * Compatible VU use
							    */
	uint16_t capabilities;     /* 15-10 0=reserved
							    *     9 1=LBA supported
							    *     8 1=DMA supported
							    *   7-0 Vendor unique
							    */

	#define DMA_SUPP (1 << 8)
	#define LBA_SUPP (1 << 9)
	uint16_t rsrv2;            /* Reserved */
	uint16_t pio_timing_mode;  /* 15-8 PIO data transfer cycle timing mode
							    *  7-0 Vendor unique
							    */
	uint16_t dma_timing_mode;  /* 15-8 DMA data transfer cycle timing mode
	 	 	 	 	 	 	    *  7-0 Vendor unique
	 	 	 	 	 	 	    */
	uint16_t valid;            /*15-1 Reserved
	                            *   0 1=the fields reported in words 54-58 are valid
	                            *     0=the fields reported in words 54-58 may be valid
	                            */
	uint16_t cur_num_cyl;      /* Number of current cylinders */
	uint16_t cur_num_head;     /* Number of current heads */
	uint16_t cur_sct_pr_track; /* Number of current sectors per track */
	uint32_t capacity;         /* Current capacity in sectors */
	uint16_t num_in_multiple;  /* 15-9 Reserved
	                            *    8 1 = Multiple sector setting is valid
	                            *  7-0 xxh = Current setting for number of sectors that can be
	                            *      transferred per interrupt on R/W multiple commands
	                            */
	uint32_t num_user_sect;    /* Total number of user addressable sectors (LBA mode only)*/
	uint16_t dma_mode;         /* 15-8 Single word DMA transfer mode active
	                            *  7-0 Single word DMA transfer modes supported (see 11-3a)
	                            */
	uint16_t multiword_dma;    /* 15-8 Multiword DMA transfer mode active
	                            *  7-0 Multiword DMA transfer modes supported (see 11-3b)
	                            */
	uint16_t rsrv3[64 + 1];    /*  64-127 Reserved */
	uint8_t  vendor_uniq[64 + 1];  /* 128-159 Vendor unique */
	                           /* 160-255 Reserved */
} dev_ide_ata_identif_t;

typedef struct _dev_ide_ata {
	node_t *dev_node;
	size_t size;
	int base_cmd_addr;
	int base_ctrl_addr;
	uint8_t  irq;
	unsigned char dev_select;
	dev_ide_ata_identif_t identification;
} dev_ide_ata_t;

typedef struct _ide_ata_bus {
	dev_ide_ata_t *dev_ide_ata;
	int base_cmd_addr;
	int base_ctrl_addr;
} ide_ata_bus_t;

typedef struct _ide_ata_slot {
	ide_ata_bus_t ide_bus[8];
} ide_ata_slot_t;

// Command and extended error information returned by the
// reg_reset(), reg_non_data_*(), reg_pio_data_in_*(),
// reg_pio_data_out_*(), reg_packet() and dma_pci_*() functions.

struct _reg_cmd_info
{
   // command code
   unsigned char cmd;         // command code
   // command parameters
   unsigned int  fr;          // feature (8 or 16 bits)
   unsigned int  sc;          // sec cnt (8 or 16 bits)
   unsigned int  sn;          // sec num (8 or 16 bits)
   unsigned int  cl;          // cyl low (8 or 16 bits)
   unsigned int  ch;          // cyl high (8 or 16 bits)
   unsigned char dh;          // device head
   unsigned char dc;          // device control
   long ns;                   // actual sector count
   int mc;                    // current multiple block setting
   unsigned char lbaSize;     // size of LBA used
      #define LBACHS 0           // last command used ATA CHS (not supported by MINDRVR)
                                 //    -or- last command was ATAPI PACKET command
      #define LBA28  28          // last command used ATA 28-bit LBA
      #define LBA48  48          // last command used ATA 48-bit LBA
   unsigned long lbaLow;      // lower 32-bits of ATA LBA
   unsigned long lbaHigh;     // upper 32-bits of ATA LBA
   // status and error regs
   unsigned char st;          // status reg
   unsigned char as;          // alt status reg
   unsigned char er ;         // error reg
   // driver error codes
   unsigned char ec;          // detailed error code
   unsigned char to;          // not zero if time out error
   // additional result info
   long totalBytesXfer;       // total bytes transfered
   long drqPackets;           // number of PIO DRQ packets
};

extern struct _reg_cmd_info reg_cmd_info;

// Configuration data for device 0 and 1
// returned by the reg_config() function.

extern int reg_config_info[2];

#define REG_CONFIG_TYPE_NONE  0
#define REG_CONFIG_TYPE_UNKN  1
#define REG_CONFIG_TYPE_ATA   2
#define REG_CONFIG_TYPE_ATAPI 3

#define COMMAND    0x01
#define CONTROL    0x00

#define PRIMARY_COMMAND_REG_BASE_ADDR    0x01F0
#define PRIMARY_CONTROL_REG_BASE_ADDR    0x03F6
#define PRIMARY_IRQ                      14

#define SECONDARY_COMMAND_REG_BASE_ADDR  0x0170
#define SECONDARY_CONTROL_REG_BASE_ADDR  0x0376
#define SECONDARY_IRQ                    15

#define TERTIARY_COMMAND_REG_BASE_ADDR   0x01E8
#define TERTIARY_CONTROL_REG_BASE_ADDR   0x03E6
#define TERTIARY_IRQ                     11

#define QUATERNARY_COMMAND_REG_BASE_ADDR 0x0168
#define QUATERNARY_CONTROL_REG_BASE_ADDR 0x0366
#define QUATERNARY_IRQ                   10

#define TMR_TIME_OUT 5

// These are the offsets into pio_reg_addrs[]

#define CB_DATA  0   // data reg         in/out cmd_blk_base1+0
#define CB_ERR   1   // error            in     cmd_blk_base1+1
#define CB_FR    1   // feature reg         out cmd_blk_base1+1
#define CB_SC    2   // sector count     in/out cmd_blk_base1+2
#define CB_SN    3   // sector number    in/out cmd_blk_base1+3
#define CB_CL    4   // cylinder low     in/out cmd_blk_base1+4
#define CB_CH    5   // cylinder high    in/out cmd_blk_base1+5
#define CB_DH    6   // device head      in/out cmd_blk_base1+6
#define CB_STAT  7   // primary status   in     cmd_blk_base1+7
#define CB_CMD   7   // command             out cmd_blk_base1+7

#define CB_ASTAT 0   // alternate status
#define CB_DC    0   // device control

#define CB_LBA_000L    3   // LBA bits 0-7
#define CB_LBA_00H0    4   // LBA bits 8-15
#define CB_LBA_0L00    5   // LBA bits 16-23
#define CB_LBA_H000    6   // LBA bits 24-27
// error reg (CB_ERR) bits

#define CB_ER_ICRC 0x80    // ATA Ultra DMA bad CRC
#define CB_ER_BBK  0x80    // ATA bad block
#define CB_ER_UNC  0x40    // ATA uncorrected error
#define CB_ER_MC   0x20    // ATA media change
#define CB_ER_IDNF 0x10    // ATA id not found
#define CB_ER_MCR  0x08    // ATA media change request
#define CB_ER_ABRT 0x04    // ATA command aborted
#define CB_ER_NTK0 0x02    // ATA track 0 not found
#define CB_ER_NDAM 0x01    // ATA address mark not found

#define CB_ER_P_SNSKEY 0xf0   // ATAPI sense key (mask)
#define CB_ER_P_MCR    0x08   // ATAPI Media Change Request
#define CB_ER_P_ABRT   0x04   // ATAPI command abort
#define CB_ER_P_EOM    0x02   // ATAPI End of Media
#define CB_ER_P_ILI    0x01   // ATAPI Illegal Length Indication

// ATAPI Interrupt Reason bits in the Sector Count reg (CB_SC)

#define CB_SC_P_TAG    0xf8   // ATAPI tag (mask)
#define CB_SC_P_REL    0x04   // ATAPI release
#define CB_SC_P_IO     0x02   // ATAPI I/O
#define CB_SC_P_CD     0x01   // ATAPI C/D

// bits 7-4 of the device/head (CB_DH) reg

#define CB_DH_LBA  0x40    // LBA bit
#define CB_DH_DEV0 0x00    // select device 0
#define CB_DH_DEV1 0x10    // select device 1
// #define CB_DH_DEV0 0xa0    // select device 0 (old definition)
// #define CB_DH_DEV1 0xb0    // select device 1 (old definition)

// bits 1-0 of the device adress (CB_DA) reg
#define CB_DA_DS0 0x02    // selected device 0 Master
#define CB_DA_DS1 0x01    // selected device 1 Slave
#define CB_LBA_MODE 0x40; // L

// status reg (CB_STAT and CB_ASTAT) bits

#define CB_STAT_BSY  0x80  // busy
#define CB_STAT_RDY  0x40  // ready
#define CB_STAT_DF   0x20  // device fault
#define CB_STAT_WFT  0x20  // write fault (old name)
#define CB_STAT_SKC  0x10  // seek complete (only SEEK command)
#define CB_STAT_SERV 0x10  // service (overlap/queued commands)
#define CB_STAT_DRQ  0x08  // data request
#define CB_STAT_CORR 0x04  // corrected (obsolete)
#define CB_STAT_IDX  0x02  // index (obsolete)
#define CB_STAT_ERR  0x01  // error (ATA)
#define CB_STAT_CHK  0x01  // check (ATAPI)

// device control reg (CB_DC) bits

#define CB_DC_HOB    0x80  // High Order Byte (48-bit LBA)
// #define CB_DC_HD15   0x00  // bit 3 is reserved
// #define CB_DC_HD15   0x08  // (old definition of bit 3)
#define CB_DC_SRST   0x04  // soft reset
#define CB_DC_NIEN   0x02  // disable interrupts

//**************************************************************
//
// Most mandtory and optional ATA commands
//
//**************************************************************

#define CMD_CFA_ERASE_SECTORS            0xC0
#define CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define CMD_CFA_TRANSLATE_SECTOR         0x87
#define CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define CMD_CHECK_POWER_MODE1            0xE5
#define CMD_CHECK_POWER_MODE2            0x98
#define CMD_DEVICE_RESET                 0x08
#define CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define CMD_FLUSH_CACHE                  0xE7
#define CMD_FLUSH_CACHE_EXT              0xEA
#define CMD_FORMAT_TRACK                 0x50
#define CMD_IDENTIFY_DEVICE              0xEC
#define CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define CMD_IDLE1                        0xE3
#define CMD_IDLE2                        0x97
#define CMD_IDLE_IMMEDIATE1              0xE1
#define CMD_IDLE_IMMEDIATE2              0x95
#define CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define CMD_NOP                          0x00
#define CMD_PACKET                       0xA0
#define CMD_READ_BUFFER                  0xE4
#define CMD_READ_DMA                     0xC8
#define CMD_READ_DMA_EXT                 0x25
#define CMD_READ_DMA_QUEUED              0xC7
#define CMD_READ_DMA_QUEUED_EXT          0x26
#define CMD_READ_MULTIPLE                0xC4
#define CMD_READ_MULTIPLE_EXT            0x29
#define CMD_READ_SECTORS                 0x20
#define CMD_READ_SECTORS_EXT             0x24
#define CMD_READ_VERIFY_SECTORS          0x40
#define CMD_READ_VERIFY_SECTORS_EXT      0x42
#define CMD_RECALIBRATE                  0x10
#define CMD_SEEK                         0x70
#define CMD_SET_FEATURES                 0xEF
#define CMD_SET_MULTIPLE_MODE            0xC6
#define CMD_SLEEP1                       0xE6
#define CMD_SLEEP2                       0x99
#define CMD_SMART                        0xB0
#define CMD_STANDBY1                     0xE2
#define CMD_STANDBY2                     0x96
#define CMD_STANDBY_IMMEDIATE1           0xE0
#define CMD_STANDBY_IMMEDIATE2           0x94
#define CMD_WRITE_BUFFER                 0xE8
#define CMD_WRITE_DMA                    0xCA
#define CMD_WRITE_DMA_EXT                0x35
#define CMD_WRITE_DMA_QUEUED             0xCC
#define CMD_WRITE_DMA_QUEUED_EXT         0x36
#define CMD_WRITE_MULTIPLE               0xC5
#define CMD_WRITE_MULTIPLE_EXT           0x39
#define CMD_WRITE_SECTORS                0x30
#define CMD_WRITE_SECTORS_EXT            0x34
#define CMD_WRITE_VERIFY                 0x3C

//**************************************************************
//
// ATA and ATAPI PIO support functions
//
//**************************************************************

// config and reset funcitons

extern ide_ata_slot_t *detection_drive(void);

extern int reg_reset( unsigned char devRtrn );

#if INCLUDE_ATAPI_PIO

// ATAPI Packet PIO function

extern int reg_packet( unsigned char dev,
                       unsigned int cpbc,
                       unsigned char * cdbBufAddr,
                       int dir,
                       long dpbc,
                       unsigned char * dataBufAddr );

#endif   // INCLUDE_ATAPI_PIO

//**************************************************************
//
// ATA and ATAPI DMA support functions
//
//**************************************************************

#if INCLUDE_ATA_DMA || INCLUDE_ATAPI_DMA

// BMIDE registers and bits

#define BM_COMMAND_REG    0            // offset to BM command reg
#define BM_CR_MASK_READ    0x00           // read from memory
#define BM_CR_MASK_WRITE   0x08           // write to memory
#define BM_CR_MASK_START   0x01           // start transfer
#define BM_CR_MASK_STOP    0x00           // stop transfer

#define BM_STATUS_REG     2            // offset to BM status reg
#define BM_SR_MASK_SIMPLEX 0x80           // simplex only
#define BM_SR_MASK_DRV1    0x40           // drive 1 can do dma
#define BM_SR_MASK_DRV0    0x20           // drive 0 can do dma
#define BM_SR_MASK_INT     0x04           // INTRQ signal asserted
#define BM_SR_MASK_ERR     0x02           // error
#define BM_SR_MASK_ACT     0x01           // active

#define BM_PRD_ADDR_LOW   4            // offset to BM prd addr reg low 16 bits
#define BM_PRD_ADDR_HIGH  6            // offset to BM prd addr reg high 16 bits

// PCI DMA setup function (usually called once).

// !!! You may not need this function in your system - see the comments
// !!! for this function in MINDRVR.C.

extern int dma_pci_config( void );

// ATA DMA functions

extern int dma_pci_lba28( unsigned char dev, unsigned char cmd,
                          unsigned int fr, unsigned int sc,
                          unsigned long lba,
                          unsigned char * bufAddr,
                          long numSect );

extern int dma_pci_lba48( unsigned char dev, unsigned char cmd,
                          unsigned int fr, unsigned int sc,
                          unsigned long lbahi, unsigned long lbalo,
                          unsigned char * bufAddr,
                          long numSect );

#endif   // INCLUDE_ATA_DMA or INCLUDE_ATAPI_DMA

#if INCLUDE_ATAPI_DMA

// ATA DMA function

extern int dma_pci_packet( unsigned char dev,
                           unsigned int cpbc,
                           unsigned char * cdbBufAddr,
                           int dir,
                           long dpbc,
                           unsigned char * dataBufAddr );

#endif   // INCLUDE_ATAPI_DMA


#endif /* ATA_H_ */
