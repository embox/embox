/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.03.2018
 */

#ifndef XEN_IPL_H_
#define XEN_IPL_H_

#ifndef __ASSEMBLER__

typedef unsigned char __ipl_t;

extern void ipl_init(void);
extern __ipl_t ipl_save(void);
extern void ipl_restore(__ipl_t ipl);

#endif /* __ASSEMBLER__ */

#endif /* XEN_IPL_H_ */
