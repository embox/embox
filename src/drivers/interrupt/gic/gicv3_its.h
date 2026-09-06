/**
 * @file
 * @brief
 *
 * @author zhugengyu
 * @date 05.09.2026
 */

#ifndef DRIVERS_INTERRUPT_GIC_GICV3_ITS_H_
#define DRIVERS_INTERRUPT_GIC_GICV3_ITS_H_

#include <stdint.h>

#include "gic_lpi.h"

/* Kernel IRQ numbers handed out by the ITS, see gic_lpi.h. */
#define GIC_LPI_QUANTITY (256 - GIC_LPI_IRQ_BASE)

/* ITS register offsets */
#define GITS_CTLR       0x0000
#define GITS_TYPER      0x0008
#define GITS_CBASER     0x0080
#define GITS_CWRITER    0x0088
#define GITS_CREADR     0x0090
#define GITS_BASER(n)   (0x0100 + (n)*8)
#define GITS_TRANSLATER 0x10040

#define GITS_CTLR_ENABLED   (1U << 0)
#define GITS_CTLR_QUIESCENT (1U << 31)

#define GITS_TYPER_PTA (1ULL << 19)

/*
 * All ITS tables and the command queue are normal cached memory the
 * ITS reads over a non-coherent port: inner shareable, read and
 * write allocate, write back.
 */
#define ITS_CACHE_INNER_SHAREABLE 1U
#define ITS_CACHE_RAWAWB          7U

/* GITS_CBASER */
#define ITS_CBASER_SIZE_SHIFT  0 /* pages - 1, 4K pages */
#define ITS_CBASER_SHARE_SHIFT 10
#define ITS_CBASER_ADDR_SHIFT  12
#define ITS_CBASER_OUTER_SHIFT 53
#define ITS_CBASER_INNER_SHIFT 59
#define ITS_CBASER_VALID       (1ULL << 63)

/* GITS_BASER<n> */
#define ITS_BASER_SIZE_SHIFT  0 /* pages - 1 */
#define ITS_BASER_PGSZ_SHIFT  8 /* 0 = 4K pages */
#define ITS_BASER_SHARE_SHIFT 10
#define ITS_BASER_ADDR_SHIFT  12
#define ITS_BASER_ENTSZ_SHIFT 48 /* bytes per entry, read-only */
#define ITS_BASER_OUTER_SHIFT 53
#define ITS_BASER_TYPE_SHIFT  56
#define ITS_BASER_INNER_SHIFT 59
#define ITS_BASER_INDIRECT    (1ULL << 62)
#define ITS_BASER_VALID       (1ULL << 63)

/* GITS_BASER<n>.Type: this implementation reports the values a
 * GIC-600 assigns on reset; the collection table slot is fixed. */
#define ITS_BASER_TYPE_DEVICE     1U
#define ITS_BASER_TYPE_COLLECTION 4U

/* GICR LPI accounting registers (redistributor frame) */
#define ITS_GICR_CTLR_ENABLE_LPIS (1U << 0)

#define ITS_PROP_IDBITS_SHIFT 0 /* INTID bits - 1 */
#define ITS_PROP_INNER_SHIFT  7
#define ITS_PROP_SHARE_SHIFT  10
#define ITS_PROP_ADDR_SHIFT   12
#define ITS_PROP_OUTER_SHIFT  56

#define ITS_PEND_INNER_SHIFT 7
#define ITS_PEND_SHARE_SHIFT 10
#define ITS_PEND_ADDR_SHIFT  16
#define ITS_PEND_OUTER_SHIFT 56
#define ITS_PEND_PTZ         (1ULL << 62)

/* LPI property table entry */
#define ITS_LPI_PROP_ENABLE 0x1
#define ITS_LPI_PROP_PRIO   0x00 /* bits [7:2], highest priority */

/*
 * Command queue entries are 32 bytes, four 64-bit words: word 0
 * carries the opcode and the device id, word 1 the event id, the ITT
 * size or the target INTID, word 2 the collection id, the ITT or
 * redistributor address and the valid bit.
 */
#define ITS_CMD_INT     0x03
#define ITS_CMD_SYNC    0x05
#define ITS_CMD_MAPD    0x08
#define ITS_CMD_MAPC    0x09
#define ITS_CMD_MAPTI   0x0a
#define ITS_CMD_INV     0x0c
#define ITS_CMD_CLEAR   0x0d
#define ITS_CMD_DISCARD 0x0f

#define ITS_CMD_DEVID_SHIFT  32
#define ITS_CMD_PINTID_SHIFT 32
#define ITS_CMD_ITT_SHIFT    8
#define ITS_CMD_RDBASE_SHIFT 16
#define ITS_CMD_VALID        (1ULL << 63)

/* Sizing of the driver-owned memory. The tables must cover the whole
 * LPI INTID space the redistributor is programmed for, regardless of
 * how few interrupts are actually allocated. */
#define ITS_CMDQ_ENTRIES       512  /* 16 KiB of 32-byte commands */
#define ITS_DEV_TABLE_ENTRIES  2048 /* covers the PCIe requester id space */
#define ITS_COLL_TABLE_ENTRIES 512  /* one 4K page of 8-byte entries */
#define ITS_ITT_ENTRIES        64   /* per device, log2 - 1 goes to MAPD */
#define ITS_ITT_POOL_SIZE      4096
#define ITS_PROP_SIZE          65536
#define ITS_PEND_SIZE          65536

/* The property and pending tables cover 2^ITS_LPI_IDBITS_MAX LPI
 * INTIDs (65536 property bytes, the pending table rounded to 64K
 * like the reference firmware setup on this board); the value
 * programmed into GICR_PROPBASER.IDbits -- which carries (number of
 * INTID bits - 1) -- must stay below it. */
#define ITS_LPI_IDBITS_MAX     16

#define ITS_MAX_DEVICES 4

/* ITS API for message-based interrupt allocation */

/* Address endpoints write their message to (GITS_TRANSLATER). */
extern uint64_t gic_its_trans_addr(void);

/* Map a PCIe function in the ITS (ITT + MAPD command). */
extern int gic_its_device_attach(uint32_t devid);

/* Bind the next free LPI to (devid, eventid); returns the kernel
 * IRQ number from the reserved window. */
extern int gic_its_event_map(uint32_t devid, uint32_t eventid);

extern void gic_its_event_unmap(uint32_t devid, uint32_t eventid);

/* Inject a message for the already mapped (devid, eventid) pair from
 * software, the way an endpoint would by writing the doorbell. */
extern int gic_its_send_int(uint32_t devid, uint32_t eventid);

/* Drop pending state the ITS may still hold for (devid, eventid). */
extern int gic_its_send_clear(uint32_t devid, uint32_t eventid);

#endif /* DRIVERS_INTERRUPT_GIC_GICV3_ITS_H_ */
