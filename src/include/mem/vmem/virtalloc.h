/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#ifndef VMEM_VIRT_ALLOC_H_
#define VMEM_VIRT_ALLOC_H_

extern void *VIRTUAL_TABLES_START;
extern size_t VIRTUAL_TABLES_LEN;

extern void *virt_alloc_table(void);

#endif /* VMEM_VIRT_ALLOC_H_ */
