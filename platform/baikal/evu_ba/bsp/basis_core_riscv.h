/**
 * *****************************************************************************
 *  @file       basis_core_riscv.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS RISC-V common definitions
 *  @version    1.3
 *  @date       2025-06-30
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef BASIS_CORE_RISCV_H
#define BASIS_CORE_RISCV_H

#include "basis_def.h"
#include "basis_core_riscv_param.h"
#include "basis_core_riscv_reg.h"

#if __RV_CLIC_EN
#define IS_CLINT_IRQ_N(__VALUE__)   (0)
#define IS_CLIC_IRQ_N(__VALUE__)    (((__VALUE__) >= 0) && ((__VALUE__) < (int32_t)__RV_NUM_INTERRUPT))
#else
#define IS_CLINT_IRQ_N(__VALUE__)   (((__VALUE__) >= 0) && ((__VALUE__) < (int32_t)__RV_NUM_INTERRUPT))
#endif

#define F_TIMER_PULSE               ((unsigned long)1000000UL)    /*!< Core timer increment frequency (Hz) */

#define TIME_CYCLES_PER_US          ((unsigned long)(F_TIMER_PULSE) / 1000000UL)    /*!< The number of TIME cycles per microsecond */
#define TIME_CYCLES_PER_MS          ((unsigned long)(F_TIMER_PULSE) / 1000UL)       /*!< The number of TIME cycles per millisecond */

/* CSR access macros */

/**
 * @brief Writes a CSR register.
 * @param csr The 12-bit CSR register address.
 * @param val The value to write.
 */
#define CSR_WRITE(csr,val)      ({                                                                          \
                                    rv_csr_t __val = (rv_csr_t)(val);                                       \
                                    __ASM volatile("csrw " STRINGIFY(csr) ", %0"::"rK"(__val):"memory");    \
                                })

/**
 * @brief Reads a CSR register.
 * @param csr The 12-bit CSR register address.
 * @return The CSR register value.
 */
#define CSR_READ(csr)           ({                                                                          \
                                    rv_csr_t __val;                                                         \
                                    __ASM volatile("csrr %0, " STRINGIFY(csr):"=r"(__val)::"memory");       \
                                    __val;                                                                  \
                                })

/**
 * @brief Sets bits in a CSR register.
 * @param csr The 12-bit CSR register address.
 * @param mask The bit mask that specifies bit positions to be set.
 */
#define CSR_SET(csr,mask)       ({                                                                          \
                                    rv_csr_t __val = (rv_csr_t)(mask);                                      \
                                    __ASM volatile("csrs " STRINGIFY(csr) ", %0"::"rK"(__val):"memory");    \
                                })

/**
 * @brief Clears bits in a CSR register.
 * @param csr The 12-bit CSR register address.
 * @param mask The bit mask that specifies bit positions to be cleared.
 */
#define CSR_CLEAR(csr,mask)     ({                                                                          \
                                    rv_csr_t __val = (rv_csr_t)(mask);                                      \
                                    __ASM volatile("csrc " STRINGIFY(csr) ", %0"::"rK"(__val):"memory");    \
                                })

/* Hardware performance monitoring functions */

/**
 * @brief Returns the number of clock cycles executed by the processor core on which the hart is running.
 * @note Can only be executed in Machine mode.
 * @returns The number of clock cycles.
 */
__STATIC_INLINE uint64_t __get_cycle(void)
{
#if defined(__RV32)
    volatile uint32_t msb, lsb, msb2;

    do
    {
        msb = CSR_READ(CSR_MCYCLEH);
        lsb = CSR_READ(CSR_MCYCLE);
        msb2 = CSR_READ(CSR_MCYCLEH);
    } while (msb != msb2);

    return ((((uint64_t)msb) << 32) | lsb);
#else
    return 0ULL;
#endif
}

/**
 * @brief Returns the number of clock cycles executed by the processor core on which the hart is running.
 * @note Can be executed in User mode or higher privilege mode.
 * @returns The number of clock cycles.
 */
__STATIC_INLINE uint64_t __get_cycle_u(void)
{
#if defined(__RV32)
    volatile uint32_t msb, lsb, msb2;

    do
    {
        msb = CSR_READ(CSR_CYCLEH);
        lsb = CSR_READ(CSR_CYCLE);
        msb2 = CSR_READ(CSR_CYCLEH);
    } while (msb != msb2);

    return ((((uint64_t)msb) << 32) | lsb);
#else
    return 0ULL;
#endif
}

/**
 * @brief Sets the number of clock cycles executed by the processor core on which the hart is running.
 *        The counter registers have an arbitrary value after the hart is reset, and can be written 
 *        with a given value.
 * @note Can only be executed in Machine mode.
 * @param value The counter value.
 */
__STATIC_INLINE void __set_cycle(uint64_t value)
{
#if defined(__RV32)
    volatile uint32_t msb = (volatile uint32_t)((value >> 32) & 0xFFFFFFFFUL);
    volatile uint32_t lsb = (volatile uint32_t)(value & 0xFFFFFFFFUL);

    CSR_WRITE(CSR_MCYCLE, 0x0UL);
    CSR_WRITE(CSR_MCYCLEH, msb);
    CSR_WRITE(CSR_MCYCLE, lsb);
#endif
}

/**
 * @brief Returns the number of instructions the hart has retired.
 * @note Can only be executed in Machine mode.
 * @returns The number of instructions.
 */
__STATIC_INLINE uint64_t __get_instret(void)
{
#if defined(__RV32)
    volatile uint32_t msb, lsb, msb2;

    do
    {
        msb = CSR_READ(CSR_MINSTRETH);
        lsb = CSR_READ(CSR_MINSTRET);
        msb2 = CSR_READ(CSR_MINSTRETH);
    } while (msb != msb2);

    return ((((uint64_t)msb) << 32) | lsb);
#else
    return 0ULL;
#endif
}

/**
 * @brief Returns the number of instructions the hart has retired.
 * @note Can be executed in User mode or higher privilege mode.
 * @returns The number of instructions.
 */
__STATIC_INLINE uint64_t __get_instret_u(void)
{
#if defined(__RV32)
    volatile uint32_t msb, lsb, msb2;

    do
    {
        msb = CSR_READ(CSR_INSTRETH);
        lsb = CSR_READ(CSR_INSTRET);
        msb2 = CSR_READ(CSR_INSTRETH);
    } while (msb != msb2);

    return ((((uint64_t)msb) << 32) | lsb);
#else
    return 0ULL;
#endif
}

/**
 * @brief Sets the number of instructions the hart has retired.
 *        The counter registers have an arbitrary value after the hart
 *        is reset, and can be written with a given value.
 * @note Can only be executed in Machine mode.
 * @param value The counter value.
 */
__STATIC_INLINE void __set_instret(uint64_t value)
{
#if defined(__RV32)
    volatile uint32_t msb = (volatile uint32_t)((value >> 32) & 0xFFFFFFFFUL);
    volatile uint32_t lsb = (volatile uint32_t)(value & 0xFFFFFFFFUL);

    CSR_WRITE(CSR_MINSTRET, 0x0UL);
    CSR_WRITE(CSR_MINSTRETH, msb);
    CSR_WRITE(CSR_MINSTRET, lsb);
#endif
}

/* Timer */

/**
 * @brief Returns the value of the time counter.
 *        There is a single time counter register for all cores.
 * @note Can only be executed in Machine mode.
 * @returns The value of the time counter.
 */
__STATIC_INLINE uint64_t __get_time(void)
{
    volatile uint32_t msb, lsb, msb2;

    do
    {
        msb = *(volatile uint32_t *)CLINT_MTIMEH;
        lsb = *(volatile uint32_t *)CLINT_MTIMEL;
        msb2 = *(volatile uint32_t *)CLINT_MTIMEH;
    } while (msb != msb2);

    return ((((uint64_t)msb) << 32) | lsb);
}

/**
 * @brief Returns the value of the time counter.
 *        There is a single time counter register for all cores.
 * @note Can be executed in User mode or higher privilege mode.
 * @returns The number of instructions.
 */
__STATIC_INLINE uint64_t __get_time_u(void)
{
#if defined(__RV32)
    volatile uint32_t msb, lsb, msb2;

    do
    {
        msb = CSR_READ(CSR_TIMEH);
        lsb = CSR_READ(CSR_TIME);
        msb2 = CSR_READ(CSR_TIMEH);
    } while (msb != msb2);

    return ((((uint64_t)msb) << 32) | lsb);
#else
    return 0ULL;
#endif
}

/**
 * @brief Returns the value of the timer compare register.
 *        There is a timer compare register dedicated to each core.
 * @note Can only be executed in Machine mode.
 * @returns The value of the timer compare register.
 */
__STATIC_INLINE uint64_t __get_timecmp(void)
{
    volatile uint32_t msb, lsb;

    msb = *(volatile uint32_t *)CLINT_MTIMECMP0H;
    lsb = *(volatile uint32_t *)CLINT_MTIMECMP0L;

    return ((((uint64_t)msb) << 32) | lsb);
}

/**
 * @brief Sets the value of the timer compare register.
 *        There is a timer compare register dedicated to each core.
 *        A machine timer interrupt becomes pending whenever mtime
 *        contains a value greater than or equal to mtimecmp.
 * @note Can only be executed in Machine mode.
 * @param value The value of the timer compare register.
 */
__STATIC_INLINE void __set_timecmp(uint64_t value)
{
    volatile uint32_t msb = (volatile uint32_t)((value >> 32) & 0xFFFFFFFFUL);
    volatile uint32_t lsb = (volatile uint32_t)(value & 0xFFFFFFFFUL);

    *(volatile uint32_t *)CLINT_MTIMECMP0L = 0xFFFFFFFFUL;  /* No smaller than old value */
    *(volatile uint32_t *)CLINT_MTIMECMP0H = msb;           /* No smaller than new value */
    *(volatile uint32_t *)CLINT_MTIMECMP0L = lsb;           /* New value */
}

/* Delay */

/**
 * @brief Makes a delay of the specified number of cycles.
 * @note The function uses RISC-V core performance monitoring counter to detect
 *       the delay timeout. It means that the function may fail if the delay 
 *       specified is close to the maximum value. On a 32-bit system the maximum
 *       value of the counter is limited to 32 bits to minimize error.
 * @note Can be executed in User mode or higher privilege mode.
 * @param cycles The number of cycles to delay.
 */
__STATIC_FORCEINLINE void __delay_cycles(unsigned long cycles)
{
    __ASM volatile (
        "   rdcycle t0          \n"
        "1:                     \n"
        "   rdcycle t1          \n"
        "   sub t1, t1, t0      \n"
        "   bltu t1, %0, 1b     \n"
        :
        : "r"(cycles)
        : "t0", "t1", "memory"
    );
}

/**
 * @brief Makes a delay of the specified number of microseconds.
 * @note The function uses RISC-V core time counter to detect the delay timeout.
 * @note The core time counter is driven by the internal clock running at
 *       F_TIMER_PULSE frequency and synchronous with CCLK.
 * @note Can be executed in User mode or higher privilege mode.
 * @param us The number of microseconds to delay.
 */
__STATIC_FORCEINLINE void __delay_us(unsigned long us)
{
    __ASM volatile (
        "   rdtime t1           \n"
        "1:                     \n"
        "   mv t0, t1           \n"
        "   beqz %1, 3f         \n"
        "   add %1, %1, -1      \n"
        "2:                     \n"
        "   rdtime t1           \n"
        "   sub t2, t1, t0      \n"
        "   bltu t2, %2, 2b     \n"
        "   j 1b                \n"
        "3:                     \n"
        : "=r"(us)
        : "0"(us), "r"(TIME_CYCLES_PER_US)
        : "t0", "t1", "t2", "memory"
    );
}

/**
 * @brief Makes a delay of the specified number of milliseconds.
 * @note The core time counter is driven by the internal clock running at
 *       F_TIMER_PULSE frequency and synchronous with CCLK.
 * @note Can be executed in User mode or higher privilege mode.
 * @param ms The number of milliseconds to delay.
 */
__STATIC_FORCEINLINE void __delay_ms(unsigned long ms)
{
    __ASM volatile (
        "   rdtime t1           \n"
        "1:                     \n"
        "   mv t0, t1           \n"
        "   beqz %1, 3f         \n"
        "   add %1, %1, -1      \n"
        "2:                     \n"
        "   rdtime t1           \n"
        "   sub t2, t1, t0      \n"
        "   bltu t2, %2, 2b     \n"
        "   j 1b                \n"
        "3:                     \n"
        : "=r"(ms)
        : "0"(ms), "r"(TIME_CYCLES_PER_MS)
        : "t0", "t1", "t2", "memory"
    );
}

/* Basic Local Interrupt Controller (CLINT) */

/**
 * @brief Enables interrupts.
 * @note Can only be executed in Machine mode.
 */
__STATIC_FORCEINLINE void __enable_irq(void)
{
    CSR_SET(CSR_MSTATUS, CSR_MSTATUS_MIE_Msk);
}

/**
 * @brief Disables interrupts (Machine mode).
 * @note Can only be executed in Machine mode.
 */
__STATIC_FORCEINLINE void __disable_irq(void)
{
    CSR_CLEAR(CSR_MSTATUS, CSR_MSTATUS_MIE_Msk);
}

/**
 * @brief Sets interrupt handling mode and trap base address.
 *        It is recommended to disable interrupts globally
 *        prior to changing mtvec.
 * @param addr The trap base address. Should be 4 bytes aligned.
 * @param mode The interrupt handling mode. Can be one of the following values:
 *             @ref CSR_MTVEC_MODE_CLINT_DIRECT
 *             @ref CSR_MTVEC_MODE_CLINT_VECTORED
 * @note Can only be executed in Machine mode.
 */
__STATIC_INLINE void CLINT_SetBaseAddr(unsigned long addr, unsigned int mode)
{
    if ((mode == CSR_MTVEC_MODE_CLINT_DIRECT) ||
        (mode == CSR_MTVEC_MODE_CLINT_VECTORED))
    {
        /* CLINT mode */
        CSR_WRITE(CSR_MTVEC, (addr & CSR_MTVEC_CLINT_BASE_Msk)|mode);
    }
}

/**
 * @brief Enables a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLINT_EnableIRQ(int32_t IRQn)
{
    if (IS_CLINT_IRQ_N(IRQn))
    {
        CSR_SET(CSR_MIE, (1UL << IRQn));
    }
}

/**
 * @brief Disables a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLINT_DisableIRQ(int32_t IRQn)
{
    if (IS_CLINT_IRQ_N(IRQn))
    {
        CSR_CLEAR(CSR_MIE, (1UL << IRQn));
    }
}

/**
 * @brief Checks if a device specific interrupt is enabled.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns 1 if the interrupt is enabled, otherwise 0.
 */
__STATIC_INLINE uint32_t CLINT_IsEnabledIRQ(int32_t IRQn)
{
    uint32_t retval = 0UL;

    if (IS_CLINT_IRQ_N(IRQn))
    {
        retval = (uint32_t)((CSR_READ(CSR_MIE) & (1UL << IRQn)) >> IRQn);
    }

    return retval;
}

/**
 * @brief Sets the pending bit of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLINT_SetPendingIRQ(int32_t IRQn)
{
    if (IS_CLINT_IRQ_N(IRQn))
    {
        unsigned long mask = (1UL << IRQn);

        if ((mask & CSR_MIP_MSIP_Msk) != 0UL)
        {
            /* MSIP is accesed in memory-mapped control register */
            *(volatile uint32_t *)CLINT_MSIP0 |= CLINT_MSIP0_IP_Msk;
        }
        else
        {
            CSR_SET(CSR_MIP, (1UL << IRQn));
        }
    }
}

/**
 * @brief Returns pending bit for a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns 1 if the interrupt status is pending, otherwise 0.
 */
__STATIC_INLINE uint32_t CLINT_GetPendingIRQ(int32_t IRQn)
{
    uint32_t retval = 0UL;

    if (IS_CLINT_IRQ_N(IRQn))
    {
        unsigned long mask = (1UL << IRQn);

        if ((mask & CSR_MIP_MSIP_Msk) == CSR_MIP_MSIP_Msk)
        {
            /* MSIP is accesed in memory-mapped control register */
            retval = (((*(volatile uint32_t *)CLINT_MSIP0) & CLINT_MSIP0_IP_Msk) >> CLINT_MSIP0_IP_Pos);
        }
        else
        {
            retval = (uint32_t)((CSR_READ(CSR_MIP) & mask) >> IRQn);
        }
    }

    return retval;
}

/**
 * @brief Clears the pending bit of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLINT_ClearPendingIRQ(int32_t IRQn)
{
    if (IS_CLINT_IRQ_N(IRQn))
    {
        unsigned long mask = (1UL << IRQn);

        if ((mask & CSR_MIP_MSIP_Msk) != 0UL)
        {
            /* MSIP is accesed in memory-mapped control register */
            *(volatile uint32_t *)CLINT_MSIP0 &= ~CLINT_MSIP0_IP_Msk;
        }
        else
        {
            CSR_CLEAR(CSR_MIP, (1UL << IRQn));
        }
    }
}

/* Core-Local Interrupt Controller (CLIC) */

#if __RV_CLIC_EN

/**
 * @brief Sets interrupt handling mode and trap base address for direct mode.
 * @param addr The trap base address. Should be 64 bytes aligned.
 * @param mode The interrupt handling mode. Can be one of the following values:
 *             @ref CSR_MTVEC_MODE_CLIC
 * @note Can only be executed in Machine mode.
 */
__STATIC_INLINE void CLIC_SetBaseAddr(unsigned long addr, unsigned int mode)
{
    if (mode == CSR_MTVEC_MODE_CLIC)
    {
        /* Set mode and base address for direct mode */
        CSR_WRITE(CSR_MTVEC, (addr & CSR_MTVEC_CLIC_BASE_Msk)|mode);
    }
}

/**
 * @brief Sets trap vector base address for vectored mode.
 * @param addr The trap vector base address. Should be 64 bytes aligned.
 * @note Can only be executed in Machine mode.
 */
__STATIC_INLINE void CLIC_SetVectorBaseAddr(unsigned long addr)
{
    /* Set trap vector table address for vectored mode */
    CSR_WRITE(CSR_MTVT, (addr & CSR_MTVT_BASE_Msk));
}

/**
 * @brief Sets nlbits value in CLICCFG register in M-mode.
 *        nlbits define the number of bits assigned to encode the interrupt level.
 *        Incoming interrupts with a higher interrupt level can preempt an active
 *        interrupt handler running at a lower interrupt level in the same
 *        privilege mode, provided interrupts are globally enabled in this
 *        privilege mode.
 * @note The maximal value is limited by the __RV_CLICCFGLBITS parameter.
 * @param nlbits The number of nlbits.
 */
__STATIC_INLINE void CLIC_SetCfgNlbits(uint8_t nlbits)
{
    uint8_t max_val = (1U << __RV_CLICCFGLBITS) - 1U;
    uint8_t bits = (nlbits < max_val) ? nlbits : max_val;

    MODIFY_REG(*(volatile uint8_t *)CLIC_CFG,
               CLIC_CFG_NLBITS_Msk,
               (bits << CLIC_CFG_NLBITS_Pos) & CLIC_CFG_NLBITS_Msk);
}

/**
 * @brief Returns nlbits value in CLICCFG register in M-mode.
 *        nlbits define the number of bits assigned to encode the interrupt level.
 *        Incoming interrupts with a higher interrupt level can preempt an active
 *        interrupt handler running at a lower interrupt level in the same
 *        privilege mode, provided interrupts are globally enabled in this
 *        privilege mode.
 * @note The maximal value is limited by the __RV_CLICCFGLBITS parameter.
 * @return The number of nlbits.
 */
__STATIC_INLINE uint8_t CLIC_GetCfgNlbits(void)
{
    return ((uint8_t)((*(volatile uint8_t *)CLIC_CFG & CLIC_CFG_NLBITS_Msk) >> CLIC_CFG_NLBITS_Pos));
}

/**
 * @brief Sets nmbits value in CLICCFG register in M-mode.
 *        nmbits define the number of bits assigned to encode the interrupt
 *        privilege mode.
 * @note The maximal value is limited by the __RV_CLICCFGMBITS parameter.
 * @param nmbits The number of nmbits.
 */
__STATIC_INLINE void CLIC_SetCfgNmbits(uint8_t nmbits)
{
    uint8_t max_val = (1U << __RV_CLICCFGMBITS) - 1U;
    uint8_t bits = (nmbits < max_val) ? nmbits : max_val;

    MODIFY_REG(*(volatile uint8_t *)CLIC_CFG,
               CLIC_CFG_NMBITS_Msk,
               (bits << CLIC_CFG_NMBITS_Pos) & CLIC_CFG_NMBITS_Msk);
}

/**
 * @brief Returns nmbits value in CLICCFG register in M-mode.
 *        nmbits define the number of bits assigned to encode the interrupt
 *        privilege mode.
 * @note The maximal value is limited by the __RV_CLICCFGMBITS parameter.
 * @return The number of nmbits.
 */
__STATIC_INLINE uint8_t CLIC_GetCfgNmbits(void)
{
    return ((uint8_t)((*(volatile uint8_t *)CLIC_CFG & CLIC_CFG_NMBITS_Msk) >> CLIC_CFG_NMBITS_Pos));
}

/**
 * @brief Configures CLIC in M-mode.
 * @param nmbits The number of nmbits. nmbits define the number of bits assigned 
 *               to encode the interrupt privilege mode. The maximal value is
 *               limited by the __RV_CLICCFGMBITS parameter.
 * @param nlbits The number of nlbits. nlbits define the number of bits assigned
 *               to encode the interrupt level. The maximal value is limited by
 *               the __RV_CLICCFGLBITS parameter.
 */
__STATIC_INLINE void CLIC_Config(uint8_t nmbits, uint8_t nlbits)
{
    /* The number of mbits */
    uint8_t max_val = (1U << __RV_CLICCFGMBITS) - 1U;
    uint8_t mbits = (nmbits < max_val) ? nmbits : max_val;

    /* The number of lbits */
    max_val = (1U << __RV_CLICCFGLBITS) - 1U;
    uint8_t lbits = (nlbits < max_val) ? nlbits : max_val;

    MODIFY_REG(*(volatile uint8_t *)CLIC_CFG,
               (CLIC_CFG_NMBITS_Msk | CLIC_CFG_NLBITS_Msk),
               ((mbits << CLIC_CFG_NMBITS_Pos) & CLIC_CFG_NMBITS_Msk) |
               ((lbits << CLIC_CFG_NLBITS_Pos) & CLIC_CFG_NLBITS_Msk));
}

/**
 * @brief Sets interrupt level of a device specific interrupt.
 *        Interrupt level is used to determine preemption (for nesting interrupts).
 *        A parameterized number of upper bits in CLICINTCTL are assigned to 
 *        encode the interrupt level. The number of bits used for interrupt level 
 *        is defined in in NLBITS field of CLICCFG register.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param level The interrupt level.
 */
__STATIC_INLINE void CLIC_SetLevel(int32_t IRQn, uint8_t level)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        uint8_t nlbits = CLIC_GetCfgNlbits();

        if (nlbits > __RV_CLICINTCTLBITS)
            nlbits = __RV_CLICINTCTLBITS;

        if (nlbits > 0U)
        {
            uint8_t max_level = (1U << nlbits) - 1U;

            if (level > max_level)
                level = max_level;

            uint8_t mask = (0xFFU >> __RV_CLICINTCTLBITS);

            MODIFY_REG(*(volatile uint8_t *)(CLIC_INTCTL + 4 * IRQn),
                    (max_level << (8U - nlbits)),
                    (level << (8U - nlbits))|mask);
        }
    }
}

/**
 * @brief Returns interrupt level of a device specific interrupt.
 *        Interrupt level is used to determine preemption (for nesting interrupts).
 *        A parameterized number of upper bits in CLICINTCTL are assigned to 
 *        encode the interrupt level. The number of bits used for interrupt level 
 *        is defined in in NLBITS field of CLICCFG register.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns The interrupt level.
 */
__STATIC_INLINE uint8_t CLIC_GetLevel(int32_t IRQn)
{
    uint8_t level = 0U;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        uint8_t nlbits = CLIC_GetCfgNlbits();

        if (nlbits > __RV_CLICINTCTLBITS)
            nlbits = __RV_CLICINTCTLBITS;

        if (nlbits > 0U)
        {
            uint8_t max_level = (1U << nlbits) - 1U;

            level = (((*(volatile uint8_t *)(CLIC_INTCTL + 4 * IRQn)) >> (8U - nlbits)) & max_level);
        }
    }

    return level;
}

/**
 * @brief Sets interrupt level threshold in M-mode.
 * @param threshold The interrupt level threshold.
 */
__STATIC_INLINE void CLIC_SetLevelThreshold(uint8_t threshold)
{
    uint8_t nlbits = CLIC_GetCfgNlbits();

    if (nlbits > __RV_CLICINTCTLBITS)
        nlbits = __RV_CLICINTCTLBITS;

    if (nlbits > 0U)
    {
        uint8_t max_threshold = (1U << nlbits) - 1U;

        if (threshold > max_threshold)
            threshold = max_threshold;
    }
    else
    {
        threshold = 0U;
    }

    CSR_WRITE(CSR_MINTTHRESH, (((uint32_t)(threshold << (8U - nlbits))) << CSR_MINTTHRESH_TH_Pos) & CSR_MINTTHRESH_TH_Msk);
}

/**
 * @brief Returns interrupt level threshold in M-mode.
 * @returns The interrupt level threshold.
 */
__STATIC_INLINE uint8_t CLIC_GetLevelThreshold(void)
{
    uint8_t threshold = 0U;

    uint8_t nlbits = CLIC_GetCfgNlbits();

    if (nlbits > __RV_CLICINTCTLBITS)
        nlbits = __RV_CLICINTCTLBITS;

    if (nlbits > 0U)
    {
        uint8_t max_threshold = (1U << nlbits) - 1U;

        threshold = (uint8_t)((((CSR_READ(CSR_MINTTHRESH) & CSR_MINTTHRESH_TH_Msk) >> CSR_MINTTHRESH_TH_Pos) >> (8U - nlbits)) & max_threshold);
    }

    return threshold;
}

/**
 * @brief Sets interrupt priority of a device specific interrupt.
 *        Interrupt priority does not affect preemption but is only used
 *        as a tie-breaker when there are multiple pending interrupts with 
 *        the same interrupt level.
 *        The least significant bits in CLICINTCTL that are not configured 
 *        to be the part of the interrupt level are interrupt priority.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param priority The interrupt priority.
 */
__STATIC_INLINE void CLIC_SetPriority(int32_t IRQn, uint8_t priority)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        uint8_t nlbits = CLIC_GetCfgNlbits();

        if (nlbits < __RV_CLICINTCTLBITS)
        {
            uint8_t max_priority = (1U << (__RV_CLICINTCTLBITS - nlbits)) - 1U;

            if (priority > max_priority)
                priority = max_priority;

            uint8_t mask = (0xFFU >> __RV_CLICINTCTLBITS);

            MODIFY_REG(*(volatile uint8_t *)(CLIC_INTCTL + 4 * IRQn),
                    (max_priority << __RV_CLICINTCTLBITS),
                    (priority << (8U - __RV_CLICINTCTLBITS))|mask);
        }
    }
}

/**
 * @brief Returns interrupt priority of a device specific interrupt.
 *        Interrupt priority does not affect preemption but is only used
 *        as a tie-breaker when there are multiple pending interrupts with 
 *        the same interrupt level.
 *        The least significant bits in CLICINTCTL that are not configured 
 *        to be the part of the interrupt level are interrupt priority.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns The interrupt priority.
 */
__STATIC_INLINE uint8_t CLIC_GetPriority(int32_t IRQn)
{
    uint8_t priority = 0U;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        uint8_t nlbits = CLIC_GetCfgNlbits();

        if (nlbits < __RV_CLICINTCTLBITS)
        {
            uint8_t max_priority = (1U << (__RV_CLICINTCTLBITS - nlbits)) - 1U;

            priority = (((*(volatile uint8_t *)(CLIC_INTCTL + 4 * IRQn)) >> (8U - __RV_CLICINTCTLBITS)) & max_priority);
        }
    }

    return priority;
}

/**
 * @brief Checks if selective interrupt hardware vectoring feature is supported.
 * @retval 1 if the feature is supported, otherwise 0.
 */
__STATIC_INLINE uint32_t CLIC_IsSupportedVectorMode(void)
{
    return ((READ_BIT(*(volatile uint8_t *)CLIC_CFG, CLIC_CFG_NVBITS_Msk) != 0U) ? 1UL : 0UL);
}

/**
 * @brief Sets vectoring mode for a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param mode The vectoring mode. Can be one of the following values:
 *             @ref CLIC_INTATTR_SHV_DIRECT
 *             @ref CLIC_INTATTR_SHV_VECTORED
 */
__STATIC_INLINE void CLIC_SetVectorMode(int32_t IRQn, uint8_t mode)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        MODIFY_REG(*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn),
                   CLIC_INTATTR_SHV_Msk,
                   mode);
    }
}

/**
 * @brief Returns vectoring mode of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns The vectoring mode. Can be one of the following values:
 *          @ref CLIC_INTATTR_SHV_DIRECT
 *          @ref CLIC_INTATTR_SHV_VECTORED
 */
__STATIC_INLINE uint8_t CLIC_GetVectorMode(int32_t IRQn)
{
    uint8_t retval = CLIC_INTATTR_SHV_DIRECT;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        retval = (*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn) & CLIC_INTATTR_SHV_Msk);
    }

    return retval;
}

/**
 * @brief Sets trigger type for a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param type The trigger type. Can be one of the following values:
 *             @ref CLIC_INTATTR_TRIG_TYPE_LEVEL
 *             @ref CLIC_INTATTR_TRIG_TYPE_EDGE
 */
__STATIC_INLINE void CLIC_SetTrigType(int32_t IRQn, uint8_t type)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        MODIFY_REG(*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn),
                   CLIC_INTATTR_TRIG_TYPE_Msk,
                   type);
    }
}

/**
 * @brief Returns trigger type of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns The trigger type. Can be one of the following values:
 *          @ref CLIC_INTATTR_TRIG_TYPE_LEVEL
 *          @ref CLIC_INTATTR_TRIG_TYPE_EDGE
 */
__STATIC_INLINE uint8_t CLIC_GetTrigType(int32_t IRQn)
{
    uint8_t retval = CLIC_INTATTR_TRIG_TYPE_LEVEL;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        retval = (*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn) & CLIC_INTATTR_TRIG_TYPE_Msk);
    }

    return retval;
}

/**
 * @brief Sets trigger polarity for a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param polarity The trigger polarity. Can be one of the following values:
 *                 @ref CLIC_INTATTR_TRIG_POL_P
 *                 @ref CLIC_INTATTR_TRIG_POL_N
 */
__STATIC_INLINE void CLIC_SetTrigPolarity(int32_t IRQn, uint8_t polarity)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        MODIFY_REG(*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn),
                   CLIC_INTATTR_TRIG_POL_Msk,
                   polarity);
    }
}

/**
 * @brief Returns trigger polarity of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns The trigger polarity. Can be one of the following values:
 *          @ref CLIC_INTATTR_TRIG_POL_P
 *          @ref CLIC_INTATTR_TRIG_POL_N
 */
__STATIC_INLINE uint8_t CLIC_GetTrigPolarity(int32_t IRQn)
{
    uint8_t retval = CLIC_INTATTR_TRIG_POL_P;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        retval = (*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn) & CLIC_INTATTR_TRIG_POL_Msk);
    }

    return retval;
}

/**
 * @brief Sets privilege mode for a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param mode The privilege mode. Can be one of the following values:
 *             @ref CLIC_INTATTR_MODE_USER
 *             @ref CLIC_INTATTR_MODE_SUPERVISOR
 *             @ref CLIC_INTATTR_MODE_MACHINE
 * @note Not all interrupt privilege modes have to be supported by the device.
 */
__STATIC_INLINE void CLIC_SetPrivMode(int32_t IRQn, uint8_t mode)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        MODIFY_REG(*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn),
                   CLIC_INTATTR_MODE_Msk,
                   mode);
    }
}

/**
 * @brief Returns privilege mode of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns The privilege mode. Can be one of the following values:
 *          @ref CLIC_INTATTR_MODE_USER
 *          @ref CLIC_INTATTR_MODE_SUPERVISOR
 *          @ref CLIC_INTATTR_MODE_MACHINE
 */
__STATIC_INLINE uint8_t CLIC_GetPrivMode(int32_t IRQn)
{
    uint8_t retval = CLIC_INTATTR_MODE_USER;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        retval = (*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn) & CLIC_INTATTR_MODE_Msk);
    }

    return retval;
}

/**
 * @brief Configures a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @param priv_mode The privilege mode. Can be one of the following values:
 *                  @ref CLIC_INTATTR_MODE_USER
 *                  @ref CLIC_INTATTR_MODE_SUPERVISOR
 *                  @ref CLIC_INTATTR_MODE_MACHINE
 *                  @note Not all interrupt privilege modes have to be supported
 *                        by the device.
 * @param level The interrupt level.
 * @param priority The interrupt priority.
 * @param vector_mode The vectoring mode. Can be one of the following values:
 *                    @ref CLIC_INTATTR_SHV_DIRECT
 *                    @ref CLIC_INTATTR_SHV_VECTORED
 *                    @note Use CLIC_IsSupportedVectorMode() function to check
 *                          if selective interrupt hardware vectoring feature
 *                          is supported.
 * @param type The trigger type. Can be one of the following values:
 *             @ref CLIC_INTATTR_TRIG_TYPE_LEVEL
 *             @ref CLIC_INTATTR_TRIG_TYPE_EDGE
 * @param polarity The trigger polarity. Can be one of the following values:
 *                 @ref CLIC_INTATTR_TRIG_POL_P
 *                 @ref CLIC_INTATTR_TRIG_POL_N
 */
__STATIC_INLINE void CLIC_ConfigIRQ(int32_t IRQn,
                                    uint8_t priv_mode,
                                    uint8_t level,
                                    uint8_t priority,
                                    uint8_t vector_mode,
                                    uint8_t type,
                                    uint8_t polarity)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        MODIFY_REG(*(volatile uint8_t *)(CLIC_INTATTR + 4 * IRQn),
                   (CLIC_INTATTR_MODE_Msk | CLIC_INTATTR_TRIG_POL_Msk |
                   CLIC_INTATTR_TRIG_TYPE_Msk | CLIC_INTATTR_SHV_Msk),
                   (priv_mode & CLIC_INTATTR_MODE_Msk) |
                   (polarity & CLIC_INTATTR_TRIG_POL_Msk) |
                   (type & CLIC_INTATTR_TRIG_TYPE_Msk) |
                   (vector_mode & CLIC_INTATTR_SHV_Msk));

        uint8_t intctl = 0U;

        uint8_t nlbits = CLIC_GetCfgNlbits();

        if (nlbits > __RV_CLICINTCTLBITS)
            nlbits = __RV_CLICINTCTLBITS;

        if (nlbits > 0U)
        {
            uint8_t max_level = (1U << nlbits) - 1U;

            if (level > max_level)
                level = max_level;

            intctl = (level << (8U - nlbits));
        }

        if (nlbits < __RV_CLICINTCTLBITS)
        {
            uint8_t max_priority = (1U << (__RV_CLICINTCTLBITS - nlbits)) - 1U;

            if (priority > max_priority)
                priority = max_priority;

            intctl |= (priority << (8U - __RV_CLICINTCTLBITS));
        }

        intctl |= (0xFFU >> __RV_CLICINTCTLBITS);

        WRITE_REG(*(volatile uint8_t *)(CLIC_INTCTL + 4 * IRQn), intctl);
    }
}

/**
 * @brief Enables a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLIC_EnableIRQ(int32_t IRQn)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        *(volatile uint8_t *)(CLIC_INTIE + 4 * IRQn) |= CLIC_INTIE_IE_Msk;
    }
}

/**
 * @brief Disables a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLIC_DisableIRQ(int32_t IRQn)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        *(volatile uint8_t *)(CLIC_INTIE + 4 * IRQn) &= ~CLIC_INTIE_IE_Msk;
    }
}

/**
 * @brief Checks if a device specific interrupt is enabled.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns 1 if the interrupt is enabled, otherwise 0.
 */
__STATIC_INLINE uint32_t CLIC_IsEnabledIRQ(int32_t IRQn)
{
    uint32_t retval = 0UL;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        retval = ((((*(volatile uint8_t *)(CLIC_INTIE + 4 * IRQn)) & CLIC_INTIE_IE_Msk) != 0U) ? 1UL : 0UL);
    }

    return retval;
}

/**
 * @brief Sets the pending bit of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLIC_SetPendingIRQ(int32_t IRQn)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        *(volatile uint8_t *)(CLIC_INTIP + 4 * IRQn) |= CLIC_INTIP_IP_Msk;
    }
}

/**
 * @brief Returns pending bit for a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 * @returns 1 if the interrupt status is pending, otherwise 0.
 */
__STATIC_INLINE uint8_t CLIC_GetPendingIRQ(int32_t IRQn)
{
    uint8_t retval = 0U;

    if (IS_CLIC_IRQ_N(IRQn))
    {
        retval = (uint8_t)((*(volatile uint8_t *)(CLIC_INTIP + 4 * IRQn)) & CLIC_INTIP_IP_Msk);
    }

    return retval;
}

/**
 * @brief Clears the pending bit of a device specific interrupt.
 * @param IRQn The number of the interrupt. IRQn must not be negative.
 */
__STATIC_INLINE void CLIC_ClearPendingIRQ(int32_t IRQn)
{
    if (IS_CLIC_IRQ_N(IRQn))
    {
        *(volatile uint8_t *)(CLIC_INTIP + 4 * IRQn) &= ~CLIC_INTIP_IP_Msk;
    }
}

#endif /* __RV_CLIC_EN */

/* Non-Maskable Interrupts */

/**
 * @brief Sets NMI trap handler address.
 * @param addr The NMI trap handler address.
 * @note Can only be executed in Machine mode.
 */
__STATIC_INLINE void NMI_SetHandlerAddr(unsigned long addr)
{
    CSR_WRITE(CSR_MNMITVEC, addr);
}

#endif /* BASIS_CORE_RISCV_H */
