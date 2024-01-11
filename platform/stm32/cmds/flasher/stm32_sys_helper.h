/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */
#ifndef _STM32_SYS_HELPER_H_
#define _STM32_SYS_HELPER_H_

#ifndef   __STATIC_FORCEINLINE             
  #define __STATIC_FORCEINLINE                   \
              __attribute__((always_inline)) static inline
#endif

#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif

#ifndef   __NO_RETURN
  #define __NO_RETURN                            __attribute__((__noreturn__))
#endif

#define __NOP()                             __ASM volatile ("nop")

/**
  \brief   Instruction Synchronization Barrier
  \details Instruction Synchronization Barrier flushes the pipeline in the processor,
           so that all instructions following the ISB are fetched from cache or memory,
           after the instruction has been completed.
 */
__STATIC_FORCEINLINE void __ISB(void) {
  __asm volatile ("isb 0xF":::"memory");
}


/**
  \brief   Data Synchronization Barrier
  \details Acts as a special kind of Data Memory Barrier.
           It completes when all explicit memory accesses before this instruction complete.
 */
__STATIC_FORCEINLINE void __DSB(void) {
  __asm volatile ("dsb 0xF":::"memory");
}


/**
  \brief   Data Memory Barrier
  \details Ensures the apparent order of the explicit memory operations before
           and after the instruction, without ensuring their completion.
 */
__STATIC_FORCEINLINE void __DMB(void) {
  __asm volatile ("dmb 0xF":::"memory");
}
#if 0
__NO_RETURN __STATIC_INLINE void __stm32_system_reset(void) {
  __DSB();                                                          /* Ensure all outstanding memory accesses included
                                                                     buffered write are completed before reset */
#if 0
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                           (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
#endif
  __DSB();                                                          /* Ensure completion of memory access */

  for(;;)                                                        /* wait until reset */
  {
    __NOP();
  }
}
#endif
#endif /* _STM32_SYS_HELPER_H_ */