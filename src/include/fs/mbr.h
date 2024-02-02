/**
 * @file
 * @brief
 *
 * @date 31 Mar 2015
 * @author Denis Deryugin
 */

#ifndef MBR_H_
#define MBR_H_

#include <stdint.h>
/*
 *	Partition table entry structure
 */
struct pt_info {
	uint8_t	active;		/* 0x80 if partition active */
	uint8_t	start_h;		/* starting head */
	uint8_t	start_cs_l;		/* starting cylinder and sector (low byte) */
	uint8_t	start_cs_h;		/* starting cylinder and sector (high byte) */
	uint8_t	type;			/* type ID byte */
	uint8_t	end_h;			/* ending head */
	uint8_t	end_cs_l;		/* ending cylinder and sector (low byte) */
	uint8_t	end_cs_h;		/* ending cylinder and sector (high byte) */
	uint8_t	start_0;		/* starting sector# (low byte) */
	uint8_t	start_1;		/* */
	uint8_t	start_2;		/* */
	uint8_t	start_3;		/* starting sector# (high byte) */
	uint8_t	size_0;			/* size of partition (low byte) */
	uint8_t	size_1;			/* */
	uint8_t	size_2;			/* */
	uint8_t	size_3;			/* size of partition (high byte) */
};

/*
 *	Master Boot Record structure
 */
struct mbr {
	uint8_t bootcode[0x1be];	/* boot sector */
	struct pt_info ptable[4];		/* four partition table structures */
	uint8_t sig_55;				/* 0x55 signature byte */
	uint8_t sig_aa;				/* 0xaa signature byte */
};

#endif /* MBR_H_ */
