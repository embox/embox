/**
 * @file
 * @brief
 *
 * @date 07.09.2012
 * @author Andrey Gazukin
 */

#ifndef IDE_H_
#define IDE_H_

#include <kernel/sched/waitq.h>

#define CDSECTORSIZE            2048

#define HD_CONTROLLERS          2
#define HD_DRIVES               4
#define HD_PARTITIONS           4

#define PAGESIZE  4096
#define MAX_PRDS                (PAGESIZE / 8)
#define MAX_DMA_XFER_SIZE       ((MAX_PRDS - 1) * PAGESIZE)

#define HDC0_IOBASE             0x01F0
#define HDC1_IOBASE             0x0170

#define HDC0_IRQ                14
#define HDC1_IRQ                15

#define HD0_DRVSEL              0x00 /* was:0xA0 */
#define HD1_DRVSEL              0x10 /* was:0xB0 */
#define HD_LBA                  0x40

#define idedelay() usleep(25)

/*
 * Controller registers
 */
#define HDC_DATA                0x0000
#define HDC_ERR                 0x0001
#define HDC_FEATURE             0x0001
#define HDC_SECTORCNT           0x0002
#define HDC_SECTOR              0x0003
#define HDC_TRACKLSB            0x0004
#define HDC_TRACKMSB            0x0005
#define HDC_DRVHD               0x0006
#define HDC_STATUS              0x0007
#define HDC_COMMAND             0x0007
#define HDC_DEVCTRL             0x0008
#define HDC_ALT_STATUS          0x0206
#define HDC_CONTROL             0x0206

/*
 * Drive commands
 */
#define HDCMD_NULL              0x00
#define HDCMD_IDENTIFY          0xEC
#define HDCMD_RESET             0x10
#define HDCMD_DIAG              0x90
#define HDCMD_READ              0x20
#define HDCMD_WRITE             0x30
#define HDCMD_PACKET            0xA0
#define HDCMD_PIDENTIFY         0xA1
#define HDCMD_MULTREAD          0xC4
#define HDCMD_MULTWRITE         0xC5
#define HDCMD_SETMULT           0xC6
#define HDCMD_READDMA           0xC8
#define HDCMD_WRITEDMA          0xCA
#define HDCMD_SETFEATURES       0xEF
#define HDCMD_FLUSHCACHE        0xE7

/*
 * Controller status
 */
#define HDCS_ERR                0x01   /* Error */
#define HDCS_IDX                0x02   /* Index */
#define HDCS_CORR               0x04   /* Corrected data */
#define HDCS_DRQ                0x08   /* Data request */
#define HDCS_DSC                0x10   /* Drive seek complete */
#define HDCS_DWF                0x20   /* Drive write fault */
#define HDCS_DRDY               0x40   /* Drive ready */
#define HDCS_BSY                0x80   /* Controller busy */

/*
 * Device control
 */
#define HDDC_HD15               0x00  /* Use 4 bits for head (not used, was 0x08) */
#define HDDC_SRST               0x04  /* Soft reset */
#define HDDC_NIEN               0x02  /* Disable interrupts */

/*
 * Feature commands
 */
#define HDFEAT_ENABLE_WCACHE    0x02  /* Enable write caching */
#define HDFEAT_XFER_MODE        0x03  /* Set transfer mode */
#define HDFEAT_DISABLE_RLA      0x55  /* Disable read-lookahead */
#define HDFEAT_DISABLE_WCACHE   0x82  /* Disable write caching */
#define HDFEAT_ENABLE_RLA       0xAA  /* Enable read-lookahead */

/*
 * Transfer modes
 */
#define HDXFER_MODE_PIO         0x00
#define HDXFER_MODE_WDMA        0x20
#define HDXFER_MODE_UDMA        0x40

/*
 * Controller error conditions
 */
#define HDCE_AMNF               0x01   /* Address mark not found */
#define HDCE_TK0NF              0x02   /* Track 0 not found */
#define HDCE_ABRT               0x04   /* Abort */
#define HDCE_MCR                0x08   /* Media change requested */
#define HDCE_IDNF               0x10   /* Sector id not found */
#define HDCE_MC                 0x20   /* Media change */
#define HDCE_UNC                0x40   /* Uncorrectable data error */
#define HDCE_BBK                0x80   /* Bad block */

/*
 * Timeouts (in ms)
 */
#define HDTIMEOUT_DRDY          5
#define HDTIMEOUT_DRQ           5
#define HDTIMEOUT_CMD           1
#define HDTIMEOUT_BUSY          60
#define HDTIMEOUT_XFER          10

/*
 * Drive interface types
 */
#define HDIF_NONE               0
#define HDIF_PRESENT            1
#define HDIF_UNKNOWN            2
#define HDIF_ATA                3
#define HDIF_ATAPI              4

/*
 * IDE media types
 */
#define IDE_FLOPPY              0x00
#define IDE_TAPE                0x01
#define IDE_CDROM               0x05
#define IDE_OPTICAL             0x07
#define IDE_SCSI                0x21
#define IDE_DISK                0x20

/*
 * ATAPI commands
 */
#define ATAPI_CMD_REQUESTSENSE  0x03
#define ATAPI_CMD_READCAPICITY  0x25
#define ATAPI_CMD_READ10        0x28

/*
 * Transfer type
 */
#define HD_XFER_IDLE            0
#define HD_XFER_READ            1
#define HD_XFER_WRITE           2
#define HD_XFER_DMA             3
#define HD_XFER_IGNORE          4

/*
 * Bus master registers
 */
#define BM_COMMAND_REG          0      /* Offset to command reg */
#define BM_STATUS_REG           2      /* Offset to status reg */
#define BM_PRD_ADDR             4      /* Offset to PRD addr reg */

/*
 * Bus master command register flags
 */
#define BM_CR_STOP              0x00   /* Stop transfer */
#define BM_CR_START             0x01   /* Start transfer */
#define BM_CR_READ              0x00   /* Read from memory */
#define BM_CR_WRITE             0x08   /* Write to memory */

/*
 * Bus master status register flags
 */
#define BM_SR_ACT               0x01   /* Active */
#define BM_SR_ERR               0x02   /* Error */
#define BM_SR_INT               0x04   /* INTRQ signal asserted */
#define BM_SR_DRV0              0x20   /* Drive 0 can do dma */
#define BM_SR_DRV1              0x40   /* Drive 1 can do dma */
#define BM_SR_SIMPLEX           0x80   /* Simplex only */

/*
 * Parameters returned by read drive parameters command
 */
struct hdparam  {
	unsigned short config;               /* General configuration bits */
	unsigned short cylinders;            /* Cylinders */
	unsigned short reserved;
	unsigned short heads;                /* Heads */
	unsigned short unfbytespertrk;       /* Unformatted bytes/track */
	unsigned short unfbytes;             /* Unformatted bytes/sector */
	unsigned short sectors;              /* Sectors per track */
	unsigned short vendorunique[3];
	char serial[20];                     /* Serial number */
	unsigned short buffertype;           /* Buffer type */
	unsigned short buffersize;           /* Buffer size, in 512-byte units */
	unsigned short necc;                 /* ECC bytes appended */
	char rev[8];                         /* Firmware revision */
	char model[40];                      /* Model name */
	unsigned char nsecperint;            /* Sectors per interrupt */
	unsigned char resv0;                 /* Reserved */
	unsigned short usedmovsd;            /* Can use double word read/write? */
	unsigned short caps;                 /* Capabilities */
	unsigned short resv1;                /* Reserved */
	unsigned short pio;                  /* PIO data transfer cycle timing (0=slow, 1=medium, 2=fast) */
	unsigned short dma;                  /* DMA data transfer cycle timing (0=slow, 1=medium, 2=fast) */
	unsigned short valid;                /* Flag valid fields to follow */
	unsigned short logcyl;               /* Logical cylinders */
	unsigned short loghead;              /* Logical heads */
	unsigned short logspt;               /* Logical sector/track */
	unsigned short cap0;                 /* Capacity in sectors (32-bit) */
	unsigned short cap1;
	unsigned short multisec;             /* Multiple sector values */
	unsigned short totalsec0;            /* Total number of user sectors */
	unsigned short totalsec1;            /*  (LBA; 32-bit value) */
	unsigned short dmasingle;            /* Single-word DMA info */
	unsigned short dmamulti;             /* Multi-word DMA info */
	unsigned short piomode;              /* Advanced PIO modes */
	unsigned short minmulti;             /* Minimum multiword xfer */
	unsigned short multitime;            /* Recommended cycle time */
	unsigned short minpio;               /* Min PIO without flow ctl */
	unsigned short miniodry;             /* Min with IORDY flow */
	unsigned short resv2[6];             /* Reserved */
	unsigned short queue_depth;          /* Queue depth */
	unsigned short resv3[4];             /* Reserved */
	unsigned short vermajor;             /* Major version number */
	unsigned short verminor;             /* Minor version number */
	unsigned short cmdset1;              /* Command set supported */
	unsigned short cmdset2;
	unsigned short cfsse;                /* Command set-feature supported extensions */
	unsigned short cfs_enable_1;         /* Command set-feature enabled */
	unsigned short cfs_enable_2;         /* Command set-feature enabled */
	unsigned short csf_default;          /* Command set-feature default */
	unsigned short dmaultra;             /* UltraDMA mode (0:5 = supported mode, 8:13 = selected mode) */

	unsigned short word89;               /* Reserved (word 89) */
	unsigned short word90;               /* Reserved (word 90) */
	unsigned short curapmvalues;         /* Current APM values */
	unsigned short word92;               /* Reserved (word 92) */
	unsigned short hw_config;            /* Hardware config */
	unsigned short words94_125[32];      /* Reserved words 94-125 */
	unsigned short last_lun;             /* Reserved (word 126) */
	unsigned short word127;              /* Reserved (word 127) */
	unsigned short dlf;                  /* Device lock function
										* 15:9  reserved
										* 8     security level 1:max 0:high
										* 7:6   reserved
										* 5     enhanced erase
										* 4     expire
										* 3     frozen
										* 2     locked
										* 1     en/disabled
										* 0     capability
										*/

	unsigned short csfo;                 /* Current set features options
										* 15:4 reserved
										* 3      auto reassign
										* 2      reverting
										* 1      read-look-ahead
										* 0      write cache
										*/

	unsigned short words130_155[26];     /* Reserved vendor words 130-155 */
	unsigned short word156;
	unsigned short words157_159[3];      /* Reserved vendor words 157-159 */
	unsigned short words160_255[96];     /* Reserved words 160-255 */
};

struct hd;

struct prd {
	unsigned long addr;
	int len;
};

typedef struct hdc  {
	int status;                          /* Controller status */

	int iobase;                          /* I/O port registers base address */
	int irq;                             /* IRQ for controller */
	int bmregbase;                       /* Busmaster register base */

	char *bufp;                          /* Buffer pointer for next transfer */
	int nsects;                          /* Number of sectors left to transfer */
	int result;                          /* Result of transfer */
	int dir;                             /* Transfer direction */
	struct hd *active;                   /* Active drive for transfer */

	struct prd *prds;                    /* PRD list for DMA transfer */
	unsigned long prds_phys;             /* Physical address of PRD list */

	struct waitq waitq;
} hdc_t;

struct partition {
	void *bdev;
	unsigned int start;
	unsigned int len;
	unsigned short bootid;
	unsigned short systid;
};

typedef struct dev_geometry {
	int cyls;
	int heads;
	int spt;
	int sectorsize;
	int sectors;
} dev_geometry_t;

typedef struct hd {
	int   idx;
	void *bdev;                /* Device */
	hdc_t *hdc;                  /* Controller */
	struct hdparam param;        /* Drive parameter block */
	int drvsel;                  /* Drive select on controller */
	int use32bits;               /* Use 32 bit transfers */
	int sectbufs;                /* Number of sector buffers */
	int lba;                     /* LBA mode */
	int iftype;                  /* IDE interface type (ATA/ATAPI) */
	int media;                   /* Device media type (hd, cdrom, ...) */
	int multsect;                /* Sectors per interrupt */
	int udmamode;                /* UltraDMA mode */
	/*
	 * Geometry
	 */
	unsigned int blks;                    /* Number of blocks on drive */

	unsigned int cyls;                    /* Number of cylinders */
	unsigned int heads;                   /* Number of heads */
	unsigned int sectors;                 /* Sectors per track */
} hd_t;

typedef struct ide_tab {
	hd_t *drive[HD_DRIVES];
} ide_tab_t;

extern struct indexator *idedisk_idx;

extern struct ide_tab *ide_get_drive(void);
extern int ide_wait(hdc_t *hdc, unsigned char mask, unsigned int timeout);
extern void ide_select_drive(hd_t *hd);

extern void pio_write_buffer(hd_t *hd, char *buffer, int size);
extern void pio_read_buffer(hd_t *hd, char *buffer, int size);
extern void hd_setup_transfer(hd_t *hd, blkno_t blkno, int nsects);

extern int create_partitions(hd_t *hd);

#endif /* IDE_H_ */
