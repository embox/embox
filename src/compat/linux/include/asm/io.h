#ifndef __ASM_IO_H__
#define __ASM_IO_H__

#include <asm/page.h>
#include <linux/types.h>

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

#define __ioremap(offset, size, flags)  ioremap(offset, size)
#define ioremap_nocache ioremap
#define ioremap_wc ioremap_nocache

static inline void iounmap(void __iomem *addr)
{
}

#endif /* __ASM_IO_H__ */
