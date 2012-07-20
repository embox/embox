/**
 * @file
 * @brief
 *
 * @date 16.07.2012
 * @author Andrey Gazukin
 */

#ifndef ATA_H_
#define ATA_H_


#define MIN_ATA_DRIVER_VERSION "0H"

#define INCLUDE_ATA_DMA   1   // not zero to include ATA_DMA

#define INCLUDE_ATAPI_PIO 1   // not zero to include ATAPI PIO

#define INCLUDE_ATAPI_DMA 1   // not zero to include ATAPI DMA

// You must supply a function that waits for an interrupt from the
// ATA controller. This function should return 0 when the interrupt
// is received and a non zero value if the interrupt is not received
// within the time out period.

//extern int SYSTEM_WAIT_INTR_OR_TIMEOUT( void );

// You must supply a function that returns a system timer value. This
// should be a value that increments at some constant rate.

//extern long SYSTEM_READ_TIMER( void );

// This defines the number of system timer ticks per second.

#define SYSTEM_TIMER_TICKS_PER_SECOND  1000000L

//********************************************************************
//
// !!! ATA controller hardware specific data
//
//********************************************************************

// ATA Command Block base address
// (the address of the ATA Data register)
#define PIO_BASE_ADDR1 ( (unsigned char *) 0x1000 )

// ATA Control Block base address
// (the address of the ATA DevCtrl
//  and AltStatus registers)
#define PIO_BASE_ADDR2 ( (unsigned char *) 0x2000 )

// BMIDE base address (address of
// the BMIDE Command register for
// the Primary or Secondary side of
// the PCI ATA controller)
#define PIO_BMIDE_BASE_ADDR ( (unsigned char *) 0x3000 )

// Size of the ATA Data register - allowed values are 8, 16 and 32
#define PIO_DEFAULT_XFER_WIDTH 16

// Interrupts or polling mode - not zero to use interrrupts
// Note: Interrupt mode is required for DMA
#define INT_DEFAULT_INTERRUPT_MODE 0

// Command time out in seconds
#define TMR_TIME_OUT 20

//**************************************************************
//
// Data that MINDRVR makes available.
//
//**************************************************************

// public interrupt handler data

extern unsigned char int_ata_status;    // ATA status read by interrupt handler

extern unsigned char int_bmide_status;  // BMIDE status read by interrupt handler

// Interrupt or Polling mode flag.

extern unsigned char int_use_intr_flag;   // not zero to use interrupts

// ATA Data register width (8, 16 or 32)

extern unsigned char pio_xfer_width;

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
} reg_cmd_info_t;

extern struct _reg_cmd_info reg_cmd_info;

// Configuration data for device 0 and 1
// returned by the reg_config() function.

extern int reg_config_info[2];

#define REG_CONFIG_TYPE_NONE  0
#define REG_CONFIG_TYPE_UNKN  1
#define REG_CONFIG_TYPE_ATA   2
#define REG_CONFIG_TYPE_ATAPI 3

//**************************************************************
//
// Global defines -- ATA register and register bits.
// command block & control block regs
//
//**************************************************************

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
#define CB_ASTAT 8   // alternate status in     ctrl_blk_base2+6
#define CB_DC    8   // device control      out ctrl_blk_base2+6

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

#define CB_DH_DS0 0x10    // selected device 0
#define CB_DH_DS1 0x01    // selected device 1

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

extern int reg_config( void );

extern int reg_reset( unsigned char devRtrn );

// ATA Non-Data command funnctions (for LBA28 and LBA48)

extern int reg_non_data_lba28( unsigned char dev, unsigned char cmd,
                               unsigned int fr, unsigned int sc,
                               unsigned long lba );

extern int reg_non_data_lba48( unsigned char dev, unsigned char cmd,
                               unsigned int fr, unsigned int sc,
                               unsigned long lbahi, unsigned long lbalo );

// ATA PIO Data In command functions (for LBA28 and LBA48)

extern int reg_pio_data_in_lba28( unsigned char dev, unsigned char cmd,
                                  unsigned int fr, unsigned int sc,
                                  unsigned long lba,
                                  unsigned char * bufAddr,
                                  long numSect, int multiCnt );

extern int reg_pio_data_in_lba48( unsigned char dev, unsigned char cmd,
                                  unsigned int fr, unsigned int sc,
                                  unsigned long lbahi, unsigned long lbalo,
                                  unsigned char * bufAddr,
                                  long numSect, int multiCnt );

// ATA PIO Data Out command functions (for LBA28 and LBA48)

extern int reg_pio_data_out_lba28( unsigned char dev, unsigned char cmd,
                                   unsigned int fr, unsigned int sc,
                                   unsigned long lba,
                                   unsigned char * bufAddr,
                                   long numSect, int multiCnt );

extern int reg_pio_data_out_lba48( unsigned char dev, unsigned char cmd,
                                   unsigned int fr, unsigned int sc,
                                   unsigned long lbahi, unsigned long lbalo,
                                   unsigned char * bufAddr,
                                   long numSect, int multiCnt );

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
