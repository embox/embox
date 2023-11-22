/**
 * @file
 * @brief TODO
 *
 * @date Sep 27, 2013
 * @author Eldar Abusalimov
 */

#ifndef PCI_TLP_H_
#define PCI_TLP_H_

#include <stdint.h>

#define TLP_TYPE_MEM        0x0 /* Memory Read/Write Request */
#define TLP_TYPE_RDLK       0x1 /* Memory Read Lock Request */
#define TLP_TYPE_IO         0x2 /* IO Read/Write Request */
#define TLP_TYPE_CFG0       0x4 /* Config Type 0 Read/Write Request */
#define TLP_TYPE_CFG1       0x5 /* Config Type 1 Read/Write Request */
#define TLP_TYPE_CPL        0xA /* Completion */
#define TLP_TYPE_CPLLK      0xB /* Completion-Locked */

#define TLP_FMT_HEADSIZE    0x1
#define TLP_FMT_DATA        0x2

#define TLP_FMT_OFFSET      29
#define TLP_TYPE_OFFSET     24
#define TLP_BUS_OFFSET      24
#define TLP_DEV_FUN_OFFSET  16

#define TLP_FMT_MSK         0x03

#define TLP_3DW_HEADER_SIZE 0x0C
#define TLP_4DW_HEADER_SIZE 0x10

extern int tlp_build_mem_wr(uint32_t *tlp, struct pci_slot_dev *dev, char bar,
    uint32_t offset, char fmt, uint32_t *buff, uint16_t len);

extern int tlp_build_mem_rd(uint32_t *tlp, struct pci_slot_dev *dev, char bar,
    uint32_t offset, char fmt, uint32_t *buff, uint16_t len);
extern int tlp_build_io_wr(char frm, uint32_t *buff, size_t data_len);
extern int tlp_build_io_rd(char frm);

extern int tlp_build_conf0_wr(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where);
extern int tlp_build_conf0_rd(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where);

extern int tlp_build_conf1_wr(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where);

extern int tlp_build_conf1_rd(uint32_t *tlp, uint32_t bus, uint32_t dev_fn,
    uint32_t where);

extern void print_tlp_packet(const unsigned char *data, int length);
#endif /* PCI_TLP_H_ */
