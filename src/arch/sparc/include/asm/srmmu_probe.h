/**
 * \file srmmu_probe.h
 * \date Jun 18, 2009
 * \author Anton Bondarev
 * \details
 */
#ifndef SRMMU_PROBE_H_
#define SRMMU_PROBE_H_

#ifndef __ASSEMBLER__
extern unsigned long pth_addr, pth_addr1;
typedef void (*functype)(void);
void mmu_func1(void);
#define MMU_RETURN(retval)  srmmu_set_mmureg(0x00000000); return (retval);

extern unsigned long ctx, pg0, pm0, pt0;
extern unsigned long page0, page1, page2;
#endif // __ASSEMBLER__

#define MMU_PRIV            SRMMU_PRIV
#define MMU_PRIV_RDONLY     SRMMU_PRIV_RDONLY

#endif /* SRMMU_PROBE_H_ */
