/**
 * @file
 * @brief
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#ifndef X86_MSR_H_
#define X86_MSR_H_

static inline void x86_read_msr(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   __asm__ __volatile__("rdmsr":"=a"(*lo),"=d"(*hi):"c"(msr));
}

static inline void x86_write_msr(uint32_t msr, uint32_t lo, uint32_t hi)
{
   __asm__ __volatile__("wrmsr"::"a"(lo),"d"(hi),"c"(msr));
}

#endif /* X86_MSR_H_ */
