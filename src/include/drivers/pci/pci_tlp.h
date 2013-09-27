/**
 * @file
 * @brief TODO
 *
 * @date Sep 27, 2013
 * @author Eldar Abusalimov
 */


#ifndef PCI_TLP_H_
#define PCI_TLP_H_

#define TLP_TYPE_MRD     0x0   /* Memory Read Request */
#define TLP_TYPE_RDLK    0x1   /* Memory Read Lock Request */
#define TLP_TYPE_MWR     0x0   /* Memory Write Request */
#define TLP_TYPE_IORD    0x2   /* IO Read Request */
#define TLP_TYPE_IOWR    0x2   /* IO Write Request */
#define TLP_TYPE_CFGRD0  0x4   /* Config Type 0 Read Request */
#define TLP_TYPE_CFGWR0  0x4   /* Config Type 0 Write Request */
#define TLP_TYPE_CFGRD1  0x5   /* Config Type 1 Read Request */
#define TLP_TYPE_CFGWR1  0x5   /* Config Type 0 Write Request */
#define TLP_TYPE_CPL     0xA   /* Completion */
#define TLP_TYPE_CPLD    0xA   /* Completion W/Data */
#define TLP_TYPE_CPLLK   0xB   /* Completion-Locked */
#define TLP_TYPE_CPLDLK  0xB   /* Completion W/Data */

#define TLP_FMT_HEADSIZE 0x1
#define TLP_FMT_DATA     0x2

#include <stdint.h>

typedef uint32_t tlp_3dw_t[3];
typedef uint32_t tlp_4dw_t[4];

extern int tlp_build_mem_wr(char frm, uint32_t *buff, size_t data_len);
extern int tlp_build_mem_rd(char frm);
extern int tlp_build_io_wr(char frm, uint32_t *buff, size_t data_len);
extern int tlp_build_io_rd(char frm);
extern int tlp_build_conf_wr(char type, char *buff, size_t data_len);
extern int tlp_build_conf_rd(char type);

#endif /* PCI_TLP_H_ */
