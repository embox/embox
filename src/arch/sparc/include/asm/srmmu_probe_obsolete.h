/**
 * @file
 *
 * @date 18.06.10
 * @author Anton Bondarev
 */
#ifndef SRMMU_PROBE_H_
#define SRMMU_PROBE_H_

#ifndef __ASSEMBLER__
extern unsigned long pth_addr, pth_addr1;
typedef void (*functype)(void);
void mmu_func1(void);
extern int mmu_probe_init(void);
extern void leon_flush_tlb_all(void);
extern int mmu_probe_map_region(uint32_t paddr, uint32_t vaddr, uint32_t size, uint32_t flags);
extern void mmu_probe_repair_table_init(unsigned long page0_addr);
extern int mmu_probe_start(void);
extern unsigned int mmu_get_page_desc(unsigned int page_addr);
extern void flush_data_cache(void);

#define MMU_RETURN(retval)  srmmu_set_mmureg(0x00000000); return (retval);

extern unsigned long ctx, pg0, pm0, pt0;
extern unsigned long page0, page1, page2;
#endif /* __ASSEMBLER__ */

#define MMU_PRIV            SRMMU_PRIV
#define MMU_PRIV_RDONLY     SRMMU_PRIV_RDONLY

#endif /* SRMMU_PROBE_H_ */
