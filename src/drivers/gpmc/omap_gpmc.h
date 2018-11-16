/**
 * @file
 *
 * @date Jul 31, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPMC_OMAP_GPMC_H_
#define SRC_DRIVERS_GPMC_OMAP_GPMC_H_

/* GPMC Registers Mapping Summary (address offsets) */
#define GPMC_REVISION           0x00
#define GPMC_SYSCONFIG          0x10
#define GPMC_SYSSTATUS          0x14
#define GPMC_IRQSTATUS          0x18
#define GPMC_IRQENABLE          0x1c
#define GPMC_TIMEOUT_CONTROL    0x40
#define GPMC_ERR_ADDRESS        0x44
#define GPMC_ERR_TYPE           0x48
#define GPMC_CONFIG             0x50
#define GPMC_STATUS             0x54
#define GPMC_PREFETCH_CONFIG1   0x1e0
#define GPMC_PREFETCH_CONFIG2   0x1e4
#define GPMC_PREFETCH_CONTROL   0x1ec
#define GPMC_PREFETCH_STATUS    0x1f0
#define GPMC_ECC_CONFIG         0x1f4
#define GPMC_ECC_CONTROL        0x1f8
#define GPMC_ECC_SIZE_CONFIG    0x1fC
#define GPMC_BCH_SWDATA         0x000002d0

#define GPMC_CS_CONFIG1         0x60
#define GPMC_CS_CONFIG2         0x64
#define GPMC_CS_CONFIG3         0x68
#define GPMC_CS_CONFIG4         0x6c
#define GPMC_CS_CONFIG5         0x70
#define GPMC_CS_CONFIG6         0x74
#define GPMC_CS_CONFIG7         0x78

#define GPMC_CS_NAND_COMMAND    0x7C
#define GPMC_CS_NAND_ADDRESS    0x80
#define GPMC_CS_NAND_DATA       0x84

#define GPMC_CS0_OFFSET         0x60
#define GPMC_CS_SIZE            0x30

#define GPMC_CHUNK_SHIFT        24 /* 16 MB */
#define GPMC_SECTION_SHIFT      28 /* 128 MB */

#define GPMC_REVISION_MAJOR(l) ((l >> 4) & 0xF)
#define GPMC_REVISION_MINOR(l) (l & 0xF)

/* Register's fields masks */
#define GPMC_CONFIG7_CSVALID    (1 << 6)

#define GPMC_CS_NUM 8

extern uint32_t gpmc_cs_reg_read(int cs, int offset);

extern void gpmc_cs_reg_write(int cs, int offset, uint32_t val);

extern uint32_t gpmc_reg_read(int offset);

extern void gpmc_reg_write(int offset, uint32_t val);

#endif /* SRC_DRIVERS_GPMC_OMAP_GPMC_H_ */
