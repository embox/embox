#ifndef __ASM_IO_H__
#define __ASM_IO_H__

#include <asm/page.h>
#include <hal/reg.h>
#include <linux/types.h>

#ifndef __iomem       /* For clean compiles in earlier kernels without __iomem annotations */
    #define __iomem
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline unsigned long virt_to_phys(volatile void *address)
{
         return (unsigned long)address;
}

static inline void *phys_to_virt(unsigned long address)
{
         return (void*)address;
}

static inline void __iomem *ioremap(unsigned long offset, unsigned long size)
{
         return (void __iomem*)offset;
}

static inline u32 readl (const volatile void __iomem *addr)
{
	return REG32_LOAD((unsigned int) addr);
}


static inline void writel (u32 val, volatile void __iomem *addr)
{
	REG32_STORE((unsigned int) addr, val);
}

#define __ioremap(offset, size, flags)  ioremap(offset, size)
#define ioremap_nocache ioremap
#define ioremap_wc ioremap_nocache

static inline void iounmap(void __iomem *addr)
{
}

#ifdef __cplusplus 
}
#endif 

#endif /* __ASM_IO_H__ */
