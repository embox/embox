/**
 * @file
 * @brief
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#ifndef X86_MSR_H_
#define X86_MSR_H_

#define IA32_APIC_BASE            0x1B

#define IA32_APIC_BASE_ENABLE_BIT 11

static inline void ia32_msr_read(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   __asm__ __volatile__("rdmsr":"=a"(*lo),"=d"(*hi):"c"(msr));
}

static inline void ia32_msr_write(uint32_t msr, uint32_t lo, uint32_t hi)
{
   __asm__ __volatile__("wrmsr"::"a"(lo),"d"(hi),"c"(msr));
}

#endif /* X86_MSR_H_ */
