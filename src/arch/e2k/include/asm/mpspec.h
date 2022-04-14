#ifndef __L_ASM_MPSPEC_H
#define __L_ASM_MPSPEC_H

/*
 * Structure definitions for SMP machines following the
 * Intel Multiprocessing Specification 1.1 and 1.4.
 */

#ifndef __ASSEMBLY__

#include <asm/bootinfo.h>
#include <endian.h>

#ifdef CONFIG_E2K
#include <asm/e2k.h>
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define SMP_FLOATING_TABLE_SIG  0x5f4d505f
#else
#define SMP_FLOATING_TABLE_SIG  0x5f504d5f
#endif


/*
 * This tag identifies where the SMP configuration
 * information is.
 */
#ifdef __LITTLE_ENDIAN
#define SMP_MAGIC_IDENT	(('_'<<24)|('P'<<16)|('M'<<8)|'_')
#elif __BIG_ENDIAN
#define SMP_MAGIC_IDENT	('_'|('P'<<8)|('M'<<16)|('_'<<24))
#else
#error not byte order defined
#endif /*__BIG_ENDIAN*/

/*
 * a maximum of NR_CPUS APICs with the current APIC ID architecture.
 * a maximum of IO-APICs is summary:
 *	each IO link can have IOHUB with IO-APIC
 *	each node can have embeded IO-APIC
 */
#define MAX_LOCAL_APICS	(NR_CPUS * 2) /* apic numbering can be with holes */
#define	MAX_IO_APICS	(MAX_NUMIOLINKS + MAX_NUMNODES)
#define	MAX_APICS	MAX_LOCAL_APICS

#define SMP_FLOATING_TABLE_LEN sizeof(struct intel_mp_floating)

#if __BYTE_ORDER == __BIG_ENDIAN
#define SMP_FLOATING_TABLE_SIG  0x5f4d505f
#else
#define SMP_FLOATING_TABLE_SIG  0x5f504d5f
#endif

struct intel_mp_floating
{
	char mpf_signature[4];		/* "_MP_" 			*/
	unsigned long mpf_physptr;	/* Configuration table address	*/
	unsigned char mpf_length;	/* Our length (paragraphs)	*/
	unsigned char mpf_specification;/* Specification version	*/
	unsigned char mpf_checksum;	/* Checksum (makes sum 0)	*/
	unsigned char mpf_feature1;	/* Standard or configuration ? 	*/
	unsigned char mpf_feature2;	/* Bit7 set for IMCR|PIC	*/
	unsigned char mpf_feature3;	/* Unused (0)			*/
	unsigned char mpf_feature4;	/* Unused (0)			*/
	unsigned char mpf_feature5;	/* Unused (0)			*/
};

#define	MPF_64_BIT_SPECIFICATION	8	/* MPF specification describe */
						/* new MP table compatible */
						/* with 64-bits arch */
#define	MP_SPEC_ADDR_ALIGN		4	/* addresses can be */
						/* word-aligned */
#define	MP_NEW_ADDR_ALIGN		8	/* all addresses should be */
						/* double-word aligned */

#define	ALIGN_BYTES_DOWN(addr, bytes)	(((addr) / (bytes)) * (bytes))
#define	ALIGN_BYTES_UP(addr, bytes)	((((addr) + (bytes)-1) / (bytes)) * \
						(bytes))
#define	MP_ALIGN_BYTES(addr, bytes)	ALIGN_BYTES_UP(addr, bytes)

#define	IS_64_BIT_MP_SPECS()		\
		(boot_mpf_found->mpf_specification == MPF_64_BIT_SPECIFICATION)
#define	MP_ADDR_ALIGN(addr)		\
		(unsigned char *)(MP_ALIGN_BYTES((unsigned long long)(addr), \
			(IS_64_BIT_MP_SPECS()) ? MP_NEW_ADDR_ALIGN : \
							MP_SPEC_ADDR_ALIGN))
#define	MP_SIZE_ALIGN(addr)		\
		MP_ALIGN_BYTES((unsigned long long)(addr), \
			(IS_64_BIT_MP_SPECS()) ? MP_NEW_ADDR_ALIGN : \
							MP_SPEC_ADDR_ALIGN)
#define enable_update_mptable 0

struct mpc_table
{
	char mpc_signature[4];
#define MPC_SIGNATURE "PCMP"
	unsigned short mpc_length;	/* Size of table */
	char  mpc_spec;			/* 0x01 */
	char  mpc_checksum;
	char  mpc_oem[8];
	char  mpc_productid[12];
	unsigned int mpc_oemptr;	/* 0 if not present */
	unsigned short mpc_oemsize;	/* 0 if not present */
	unsigned short mpc_oemcount;
	unsigned int mpc_lapic;	/* APIC address */
	unsigned short mpe_length;	/* Extended Table size */
	unsigned char mpe_checksum;	/* Extended Table checksum */
	unsigned char reserved;
};

#if __BYTE_ORDER == __BIG_ENDIAN
#define SMP_CONFIG_TABLE_SIG 0x50434d50
#else
#define SMP_CONFIG_TABLE_SIG 0x504d4350
#endif

struct mp_config_table
{
	uint32_t mpc_signature; /* "PCMP" */
	uint16_t mpc_length;       /* Size of table */
	uint8_t mpc_spec;          /* 0x01 */
	uint8_t mpc_checksum;
	uint8_t mpc_oem[8];
	uint8_t mpc_productid[12];
	uint32_t mpc_oemptr;       /* 0 if not present */
	uint16_t mpc_oemsize;      /* 0 if not present */
	uint16_t mpc_oemcount;
	uint32_t mpc_lapic;        /* APIC address */
	uint16_t mpe_length;       /* Extended Table size */
	uint8_t mpe_checksum;      /* Extended Table checksum */
	uint8_t reserved;
};
typedef	struct mp_config_table mp_config_table_t;

/* Followed by entries */

#define	MP_PROCESSOR	0
#define	MP_BUS		1
#define	MP_IOAPIC	2
#define	MP_INTSRC	3
#define	MP_LINTSRC	4
#define MP_TIMER	5
#define MP_I2C_SPI	6
#define MP_IOLINK	7
#define MP_PMC		8
#define	MP_BDEV		9
#define	MP_GPIO_ACT	10
#define	MP_IOEPIC	11

struct mpc_config_processor
{
	unsigned char mpc_type;		/* MP_PROCESSOR */
	unsigned char mpc_apicid;	/* Local APIC number */
	unsigned char mpc_apicver;	/* Its versions */
	unsigned char mpc_cpuflag;
#define CPU_ENABLED		1	/* Processor is available */
#define CPU_BOOTPROCESSOR	2	/* Processor is the BP */
	unsigned int mpc_cpufeature;
#define CPU_STEPPING_MASK 0x0F
#define CPU_MODEL_MASK	0xF0
#define CPU_FAMILY_MASK	0xF00
	unsigned int mpc_featureflag;	/* CPUID feature value */
	unsigned int mpc_cepictimerfreq;	/* Frequency of CEPIC timer */
	unsigned int mpc_reserved;
};

struct mpc_config_bus
{
	unsigned char mpc_type;		/* MP_BUS */
	unsigned char mpc_busid;
	unsigned char mpc_bustype[6];
};

/* List of Bus Type string values, Intel MP Spec. */
#define BUSTYPE_EISA	"EISA"
#define BUSTYPE_ISA	"ISA"
#define BUSTYPE_INTERN	"INTERN"	/* Internal BUS */
#define BUSTYPE_MCA	"MCA"
#define BUSTYPE_VL	"VL"		/* Local bus */
#define BUSTYPE_PCI	"PCI"
#define BUSTYPE_PCMCIA	"PCMCIA"
#define BUSTYPE_CBUS	"CBUS"
#define BUSTYPE_CBUSII	"CBUSII"
#define BUSTYPE_FUTURE	"FUTURE"
#define BUSTYPE_MBI	"MBI"
#define BUSTYPE_MBII	"MBII"
#define BUSTYPE_MPI	"MPI"
#define BUSTYPE_MPSA	"MPSA"
#define BUSTYPE_NUBUS	"NUBUS"
#define BUSTYPE_TC	"TC"
#define BUSTYPE_VME	"VME"
#define BUSTYPE_XPRESS	"XPRESS"

struct mpc_ioapic
{
	unsigned char type;	/* MP_IOAPIC */
	unsigned char apicid;
	unsigned char apicver;
	unsigned char flags;
#define MPC_APIC_USABLE		0x01
	unsigned long apicaddr;
};

struct mpc_config_ioapic {
	uint8_t type;	/* MP_IOAPIC */
	uint8_t apicid;
	uint8_t apicver;
	uint8_t flags;

	unsigned long apicaddr;
};

#if 0
struct mpc_ioepic {
	unsigned char type;	/* MP_IOEPIC */
	unsigned char epicver;
	unsigned short epicid;
	unsigned short nodeid;
	unsigned char reserved[2];
	unsigned long epicaddr;
} __packed;
#endif

#define	MPC_IOIRQFLAG_PO_BS	0x0	/* Bus specific */
#define	MPC_IOIRQFLAG_PO_AH	0x1	/* Active high  */
#define	MPC_IOIRQFLAG_PO_RES	0x2	/* Reserved     */
#define	MPC_IOIRQFLAG_PO_AL	0x3	/* Active low   */

#define	MPC_IOIRQFLAG_EL_BS	0x0	/* Bus specific     */
#define	MPC_IOIRQFLAG_EL_FS	0x4	/* Trigger by front */
#define	MPC_IOIRQFLAG_EL_RES	0x8	/* Reserved         */
#define	MPC_IOIRQFLAG_EL_LS	0xC	/* Trigger by level */

struct mpc_intsrc
{
	unsigned char type;	/* MP_INTSRC */
	unsigned char irqtype;
	unsigned short irqflag;
	unsigned char srcbus;
	unsigned char srcbusirq;
	unsigned char dstapic;
	unsigned char dstirq;
};

struct mpc_config_intsrc
{
	unsigned char type;	/* MP_INTSRC */
	unsigned char irqtype;
	unsigned short irqflag;
	unsigned char srcbus;
	unsigned char srcbusirq;
	unsigned char dstapic;
	unsigned char dstirq;
};

enum mp_irq_source_types {
	mp_INT = 0,
	mp_NMI = 1,
	mp_SMI = 2,
	mp_ExtINT = 3,
	mp_FixINT = 4	/* fixed interrupt pin for PCI */
};

#define MP_IRQDIR_DEFAULT	0
#define MP_IRQDIR_HIGH		1
#define MP_IRQDIR_LOW		3

#ifdef CONFIG_BIOS
#define MP_IRQ_POLARITY_DEFAULT	0x0
#define MP_IRQ_POLARITY_HIGH	0x1
#define MP_IRQ_POLARITY_LOW	0x3
#define MP_IRQ_POLARITY_MASK    0x3
#define MP_IRQ_TRIGGER_DEFAULT	0x0
#define MP_IRQ_TRIGGER_EDGE	0x4
#define MP_IRQ_TRIGGER_LEVEL	0xc
#define MP_IRQ_TRIGGER_MASK     0xc
#endif /* CONFIG_BIOS */


struct mpc_config_lintsrc
{
	unsigned char mpc_type;		/* MP_LINTSRC */
	unsigned char mpc_irqtype;
	unsigned short mpc_irqflag;
	unsigned char mpc_srcbusid;
	unsigned char mpc_srcbusirq;
	unsigned char mpc_destapic;
#define MP_APIC_ALL	0xFF
	unsigned char mpc_destapiclint;
};

/*
 *	Default configurations
 *
 *	1	2 CPU ISA 82489DX
 *	2	2 CPU EISA 82489DX neither IRQ 0 timer nor IRQ 13 DMA chaining
 *	3	2 CPU EISA 82489DX
 *	4	2 CPU MCA 82489DX
 *	5	2 CPU ISA+PCI
 *	6	2 CPU EISA+PCI
 *	7	2 CPU MCA+PCI
 */

#define MAX_IRQ_SOURCES	(128 * MAX_NUMIOHUBS)

/* (32 * nodes) for PCI, and one number is a special case */
#define MAX_MP_BUSSES 256

enum mp_bustype {
	MP_BUS_ISA = 1,
	MP_BUS_EISA,
	MP_BUS_PCI,
	MP_BUS_MCA
};

/*
 * IO link configurations
 */

#define	MAX_NUMIOLINKS	MACH_MAX_NUMIOLINKS
#define	MAX_NUMIOHUBS	MAX_NUMIOLINKS
#define	NODE_NUMIOLINKS	MACH_NODE_NUMIOLINKS

typedef struct mpc_config_iolink {
	unsigned char mpc_type;		/* type is MP_IOLINK */
	unsigned char mpc_iolink_type;	/* type of IO link: IOHUB or RDMA */
	unsigned short mpc_iolink_ver;	/* version of IOHUB or RDMA */
	unsigned int mpc_reserved;	/* reserved */
	int node;			/* number od node: 0 - 3 */
	int link;			/* local number of link on node: 0-1 */
	short bus_min;			/* number of root bus on IOHUB */
	short bus_max;			/* number of max bus on IOHUB */
	short apicid;			/* IO-APIC id connected to the */
					/* IOHUB */
	short mpc_reserv16;		/* reserved 16-bits value */
	unsigned long pci_mem_start;	/* PCI mem area for IOMMU v6 */
	unsigned long pci_mem_end;
} mpc_config_iolink_t;

enum mp_iolink_type {
	MP_IOLINK_IOHUB = 1,		/* IO link is IOHUB */
	MP_IOLINK_RDMA			/* IO link is RDMA controller */
};

enum mp_iolink_ver {
	MP_IOHUB_FPGA_VER	= 0x10, /* IOHUB implemented on FPGA (Altera) */
};
#define	MAX_MP_TIMERS		4

typedef struct mpc_config_timer {
	unsigned char mpc_type;		/* MP_TIMER */
	unsigned char mpc_timertype;
	unsigned char mpc_timerver;
	unsigned char mpc_timerflags;
	unsigned long mpc_timeraddr;
} mpc_config_timer_t;

enum mp_timertype {
	MP_PIT_TYPE,		/* programmed interval timer */
	MP_LT_TYPE,		/* Elbrus iohub timer */
	MP_HPET_TYPE,		/* High presicion eventualy timer */
	MP_RTC_TYPE,		/* real time clock */
	MP_PM_TYPE		/* power managment timer */
};

#define	MP_LT_VERSION		1
#define	MP_LT_FLAGS		0

#define	MP_RTC_VER_CY14B101P	2
#define MP_RTC_FLAG_SYNCINTR	0x01

typedef struct mpc_config_i2c {
	unsigned char mpc_type;		/* MP_I2C_SPI */
	unsigned char mpc_max_channel;
	unsigned char mpc_i2c_irq;
	unsigned char mpc_revision;
	unsigned long mpc_i2ccntrladdr;
	unsigned long mpc_i2cdataaddr;
} mpc_config_i2c_t;

typedef struct mpc_config_pmc {
	unsigned char mpc_type;		/* MP_PMC */
	unsigned char mpc_pmc_type; /* Izumrud or Processor-2 */
	unsigned char mpc_pmc_version;
	unsigned char mpc_pmc_vmax; /* VMAX: bits 40:34 in l_pmc.vrange */
	unsigned char mpc_pmc_vmin; /* VMIN: bits 33:27 in l_pmc.vrange */
	unsigned char mpc_pmc_fmax; /* FMAX: bits 26:20 in l_pmc.vrange */
	unsigned char reserved[2];
	unsigned long mpc_pmc_cntrl_addr; /* base of pmc regs */
	unsigned long mpc_pmc_data_addr;
	unsigned int mpc_pmc_data_size;
	unsigned int mpc_pmc_p_state[4]; /* VID 15:9, DID 8:4, FID 3:0 */
	unsigned int mpc_pmc_freq; /* Frequency in KHz */
} mpc_config_pmc_t;



typedef struct mpc_bdev {
	unsigned char mpc_type;		/* MP_BDEV */
	unsigned char mpc_bustype;	/* I2C or SPI */
	unsigned char mpc_nodeid;
	unsigned char mpc_linkid;
	unsigned char mpc_busid;
	unsigned char mpc_baddr;
	unsigned char mpc_bdev_name[16];
} mpc_bdev_t;

#define MPC_BDEV_DTYPE_I2C	1
#define MPC_BDEV_DTYPE_SPI	2

typedef struct mpc_gpio_act {
	unsigned char mpc_type;		/* MP_GPIO_ACT */
	unsigned char mpc_nodeid;
	unsigned char mpc_linkid;
	unsigned char mpc_busid;
	unsigned char mpc_gpio_pin;
	unsigned char mpc_pin_direction;
	unsigned char mpc_gpio_act_name[16];
} mpc_gpio_act_t;

#define MP_GPIO_ACT_DIRECTION_IN	1
#define MP_GPIO_ACT_DIRECTION_OUT	2

#ifdef __KERNEL__
struct iohub_sysdata;
void mp_pci_add_resources(struct list_head *resources,
			   struct iohub_sysdata *sd);
extern int __init mp_ioepic_find_bus(int ioepic_id);
#ifdef	CONFIG_IOHUB_DOMAINS
struct iohub_sysdata;
extern int mp_find_iolink_root_busnum(int node, int link);
extern int mp_find_iolink_io_apicid(int node, int link);
extern int mp_fix_io_apicid(unsigned int src_apicid, unsigned int new_apicid);
void mp_pci_add_resources(struct list_head *resources,
			   struct iohub_sysdata *sd);
extern int mp_iohubs_num;
#else
static inline int mp_fix_io_apicid(unsigned int src_apicid,
				   unsigned int new_apicid)
{
	return 0;
}
#endif /* CONFIG_IOHUB_DOMAINS */
extern int get_bus_to_io_apicid(int busnum);

#if defined(CONFIG_MCA) || defined(CONFIG_EISA)
extern int mp_bus_id_to_type [MAX_MP_BUSSES];
#endif

extern DECLARE_BITMAP(mp_bus_not_pci, MAX_MP_BUSSES);

extern struct mpc_ioapic mp_ioapics[MAX_IO_APICS];
extern unsigned int boot_cpu_physical_apicid;
extern int smp_found_config;
extern void find_smp_config(boot_info_t *bblock);
extern void get_smp_config(void);
extern int nr_ioapics;
extern int apic_version[MAX_LOCAL_APIC];
extern int mp_irq_entries;
extern struct mpc_intsrc mp_irqs [];
extern int mpc_default_type;
extern unsigned long mp_lapic_addr;
extern int pic_mode;
extern int using_apic_timer;
extern mpc_config_timer_t mp_timers[MAX_MP_TIMERS];
extern int nr_timers;
extern int rtc_model;
extern int rtc_syncintr;

#define	early_iohub_online(node, link)	mach_early_iohub_online((node), (link))
#define	early_sic_init()		mach_early_sic_init()
#endif  /* __KERNEL__ */

#ifndef CONFIG_ENABLE_BIOS_MPTABLE
#define CONFIG_ENABLE_BIOS_MPTABLE 1
#endif

#ifdef CONFIG_ENABLE_BIOS_MPTABLE
#define	MPE_SYSTEM_ADDRESS_SPACE	0x80
#define	MPE_BUS_HIERARCHY		0x81
#define	MPE_COMPATIBILITY_ADDRESS_SPACE	0x82

struct mp_exten_config {
	unsigned char mpe_type;
	unsigned char mpe_length;
};

typedef struct mp_exten_config *mpe_t;

struct mp_exten_system_address_space {
	unsigned char mpe_type;
	unsigned char mpe_length;
	unsigned char mpe_busid;
	unsigned char mpe_address_type;
#define ADDRESS_TYPE_IO       0
#define ADDRESS_TYPE_MEM      1
#define ADDRESS_TYPE_PREFETCH 2
	unsigned int  mpe_address_base_low;
	unsigned int  mpe_address_base_high;
	unsigned int  mpe_address_length_low;
	unsigned int  mpe_address_length_high;
};

struct mp_exten_bus_hierarchy {
	unsigned char mpe_type;
	unsigned char mpe_length;
	unsigned char mpe_busid;
	unsigned char mpe_bus_info;
#define BUS_SUBTRACTIVE_DECODE 1
	unsigned char mpe_parent_busid;
	unsigned char reserved[3];
};

struct mp_exten_compatibility_address_space {
	unsigned char mpe_type;
	unsigned char mpe_length;
	unsigned char mpe_busid;
	unsigned char mpe_address_modifier;
#define ADDRESS_RANGE_SUBTRACT 1
#define ADDRESS_RANGE_ADD      0
	unsigned int  mpe_range_list;
#define RANGE_LIST_IO_ISA	0
	/* X100 - X3FF
	 * X500 - X7FF
	 * X900 - XBFF
	 * XD00 - XFFF
	 */
#define RANGE_LIST_IO_VGA	1
	/* X3B0 - X3BB
	 * X3C0 - X3DF
	 * X7B0 - X7BB
	 * X7C0 - X7DF
	 * XBB0 - XBBB
	 * XBC0 - XBDF
	 * XFB0 - XFBB
	 * XFC0 - XCDF
	 */
};

/* Default local apic addr */
#define LAPIC_ADDR 0xFEE00000

#ifdef __KERNEL__
void *smp_next_mpc_entry(struct mpc_table *mc);
void *smp_next_mpe_entry(struct mpc_table *mc);

void smp_write_processor(struct mpc_table *mc,
	unsigned char apicid, unsigned char apicver,
	unsigned char cpuflag, unsigned int cpufeature,
	unsigned int featureflag, unsigned int cepictimerfreq);
void smp_write_processors(struct mpc_table *mc,
	unsigned int phys_cpu_num);
void smp_write_bus(struct mpc_table *mc,
	unsigned char id, unsigned char *bustype);
void smp_write_ioapic(struct mpc_table *mc,
	unsigned char id, unsigned char ver,
	unsigned long apicaddr);
void smp_write_ioepic(struct mpc_table *mc,
	unsigned short id, unsigned short nodeid,
	unsigned char ver, unsigned long epicaddr);
void smp_write_iolink(struct mpc_table *mc,
	int node, int link,
	short bus_min, short bus_max,
	short picid,
	unsigned long pci_mem_start, unsigned long pci_mem_end);
void smp_write_intsrc(struct mpc_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	unsigned char srcbus, unsigned char srcbusirq,
	unsigned char dstapic, unsigned char dstirq);
void smp_write_lintsrc(struct mpc_table *mc,
	unsigned char irqtype, unsigned short irqflag,
	unsigned char srcbusid, unsigned char srcbusirq,
	unsigned char destapic, unsigned char destapiclint);
void smp_write_address_space(struct mpc_table *mc,
	unsigned char busid, unsigned char address_type,
	unsigned int address_base_low, unsigned int address_base_high,
	unsigned int address_length_low, unsigned int address_length_high);
void smp_write_bus_hierarchy(struct mpc_table *mc,
	unsigned char busid, unsigned char bus_info,
	unsigned char parent_busid);
void smp_write_compatibility_address_space(struct mpc_table *mc,
	unsigned char busid, unsigned char address_modifier,
	unsigned int range_list);
unsigned char smp_compute_checksum(void *v, int len);
void smp_write_floating_table(struct intel_mp_floating *mpf);
unsigned int write_smp_table(struct intel_mp_floating *mpf, unsigned int phys_cpu_num);
void smp_i2c_spi_timer(struct mpc_table *mc,
			unsigned char timertype, unsigned char timerver,
			unsigned char timerflags, unsigned long timeraddr);
void smp_i2c_spi_dev(struct mpc_table *mc, unsigned char max_channel,
			 unsigned char irq, unsigned long i2cdevaddr);
//#define MAX_CPUS                16      /* 16 way CPU system */
#endif  /* __KERNEL__ */

/* A table (per mainboard) listing the initial apicid of each cpu. */
//extern unsigned int initial_apicid[MAX_CPUS];
#endif /* CONFIG_ENABLE_BIOS_MPTABLE */

int generic_processor_info(int apicid, int version);

#ifdef __KERNEL__
extern void print_bootblock(bootblock_struct_t *bootblock);
#endif  /* __KERNEL__ */

#ifdef CONFIG_ACPI
extern void mp_register_ioapic(int id, unsigned long address, u32 gsi_base);
extern void mp_override_legacy_irq(u8 bus_irq, u8 polarity, u8 trigger,
				   u32 gsi);
extern void mp_config_acpi_legacy_irqs(void);
struct device;
extern int mp_register_gsi(struct device *dev, u32 gsi, int edge_level,
				 int active_high_low);
extern int acpi_probe_gsi(void);
#ifdef CONFIG_L_IO_APIC
extern int mp_find_ioapic(u32 gsi);
extern int mp_find_ioapic_pin(int ioapic, u32 gsi);
#endif
#else /* !CONFIG_ACPI: */
static inline int acpi_probe_gsi(void)
{
	return 0;
}
#endif /* CONFIG_ACPI */

#if 0
/* physid definitions */
/*
 * On e2k and sparc lapics number is the same as cpus number
 * IO-APICs number is defined by MAX_IO_APICS
 * IO-APICs IDs can be placed higher than local APICs IDs or at its hole
 * so physid_t cannot be a synonim to cpumask_t.
 */
#include <linux/bitmap.h>

#define	MAX_PHYSID_NUM		(NR_CPUS + MAX_IO_APICS)
typedef struct physid_mask {
	DECLARE_BITMAP(bits, MAX_PHYSID_NUM);
} physid_mask_t;

#define PHYSID_ARRAY_SIZE	BITS_TO_LONGS(MAX_PHYSID_NUM)

#define physid_set(physid, map)		set_bit((physid), (map).bits)
#define physid_clear(physid, map)	clear_bit((physid), (map).bits)
#define physid_isset(physid, map)	test_bit((physid), (map).bits)
#define physid_test_and_set(physid, map) test_and_set_bit((physid), (map).bits)

#define physids_and(dstp, src1, src2)	\
		bitmap_and((dst).bits, (src1).bits, (src2).bits, MAX_PHYSID_NUM)

#define physids_or(dst, src1, src2)	\
		bitmap_or((dst).bits, (src1).bits, (src2).bits, MAX_PHYSID_NUM)

#define physids_clear(map)		\
		bitmap_zero((map).bits, MAX_PHYSID_NUM)

#define physids_complement(dst, src)	\
		bitmap_complement((dst).bits, (src).bits, MAX_PHYSID_NUM)

#define physids_empty(map)		\
		bitmap_empty((map).bits, MAX_PHYSID_NUM)

#define physids_equal(map1, map2)	\
		bitmap_equal((map1).bits, (map2).bits, MAX_PHYSID_NUM)

#define physids_weight(map)		\
		bitmap_weight((map).bits, MAX_PHYSID_NUM)

#define physids_shift_left(dst, src, n)	\
		bitmap_shift_left((dst).bits, (src).bits, (n), MAX_PHYSID_NUM)

static inline unsigned long physids_coerce(physid_mask_t *map)
{
	return map->bits[0];
}

static inline void physids_promote(unsigned long physids, physid_mask_t *map)
{
	physids_clear(*map);
	map->bits[0] = physids;
}

static inline void physid_set_mask_of_physid(int physid, physid_mask_t *map)
{
	physids_clear(*map);
	physid_set(physid, *map);
}

#define PHYSID_MASK_ALL		{ {[0 ... PHYSID_ARRAY_SIZE-1] = ~0UL} }
#define PHYSID_MASK_NONE	{ {[0 ... PHYSID_ARRAY_SIZE-1] = 0UL} }

extern physid_mask_t phys_cpu_present_map;
extern physid_mask_t phys_cpu_offline_map;
#endif /* if 0 */

extern int mpspec_init(void);
extern uint64_t mpspec_get_clock_base(void);
extern uintptr_t mpspec_get_uart_base(void);
extern uint64_t mpspec_get_pci_conf_base(void);

#endif	/* __ASSEMBLY__ */


#endif	/* __L_ASM_MPSPEC_H */
