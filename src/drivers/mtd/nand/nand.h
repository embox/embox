/**
 * @file
 *
 * @date Jul 30, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MTD_NAND_H_
#define SRC_DRIVERS_MTD_NAND_H_

#include <stdint.h>

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0          0x00
#define NAND_CMD_READ1          0x01
#define NAND_CMD_RNDOUT         0x05
#define NAND_CMD_PAGEPROG       0x10
#define NAND_CMD_READOOB        0x50
#define NAND_CMD_ERASE1         0x60
#define NAND_CMD_STATUS         0x70
#define NAND_CMD_SEQIN          0x80
#define NAND_CMD_RNDIN          0x85
#define NAND_CMD_READID         0x90
#define NAND_CMD_ERASE2         0xd0
#define NAND_CMD_PARAM          0xec
#define NAND_CMD_GET_FEATURES   0xee
#define NAND_CMD_SET_FEATURES   0xef
#define NAND_CMD_RESET          0xff

#define NAND_CMD_LOCK           0x2a
#define NAND_CMD_UNLOCK1        0x23
#define NAND_CMD_UNLOCK2        0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART      0x30
#define NAND_CMD_RNDOUTSTART    0xE0
#define NAND_CMD_CACHEDPROG     0x15

#define NAND_CMD_NONE           0xFF

/* Status bits */
#define NAND_STATUS_FAIL        0x01
#define NAND_STATUS_FAIL_N1     0x02
#define NAND_STATUS_TRUE_READY  0x20
#define NAND_STATUS_READY       0x40
#define NAND_STATUS_WP          0x80

struct nand_dev_ops {
	int (*nand_write)(uint8_t cmd, uint32_t addr, uint32_t data);
	int (*nand_read)(uint8_t cmd, uint32_t addr, uint32_t *data);
};

struct nand_device {
	int number;
	char nand_dev_name[16];
	struct nand_dev_ops *nand_dev_ops;
};

extern int nand_create(const char *name, const struct nand_dev_ops *);
#endif /* SRC_DRIVERS_MTD_NAND_H_ */
