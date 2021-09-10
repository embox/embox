/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    27.01.2020
 */

#include <assert.h>
#include <stdint.h>

#include <hal/arch.h>
#include <hal/clock.h>
#include <hal/ipl.h>
#include <hal/reg.h>

#include <framework/mod/options.h>
#include <module/embox/arch/system.h>

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t AHBRSTR;
    volatile uint32_t CFGR2;
    volatile uint32_t CFGR3;
    volatile uint32_t CR2;
} rcc_struct; // Doc: DocID025023 Rev 4, 125/779.

typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t RES_1;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} flash_struct; // Doc: DocID025023 Rev 4, 64/779.

// Doc: DS9773 Rev 4, 39/93.
#define RCC       ((rcc_struct *)          0x40021000)
#define FLASH     ((flash_struct *)        0x40022000)

// Nucleo hasn't HSE in board. I added it to my board own.
const uint32_t HSE_VALUE = 8000000;

void arch_init() {
    // Board is always working in max frequency.
    static_assert(OPTION_MODULE_GET(embox__arch__system, NUMBER, core_freq) == 48000000, "");

    // On HSE.
    RCC->CR |= (1 << 16); // Doc: DocID025023 Rev 4, 99/779.
    __asm("nop");
    __asm("nop");
    __asm("nop");
    while(!(RCC->CR & (1 << 17))) {};

    // Init PLL, HSE is 8 MHz.
    // Doc: DocID025023 Rev 4, 102/779.
    RCC->CFGR = (0b0100 << 18)|(1 << 16); // HSI * 3.
    __asm("nop");
    __asm("nop");
    __asm("nop");
    RCC->CR |= (1 << 24);
    __asm("nop");
    __asm("nop");
    __asm("nop");
    while(!(RCC->CR & (1 << 25))) {};

    // Set flash latency.
    // Doc: DocID025023 Rev 4, 58/779.
    // LATENCY = 1, PRFTBS = on, PRFTBE = on.
    FLASH->ACR = (1 << 0)|(1 << 4)|(1 << 5);

    // Set PLL by CPUCLK
    RCC->CFGR |= (0b10 << 0);
    __asm("nop");
    __asm("nop");
    __asm("nop");
    while(!((RCC->CFGR & (0b11 << 2)) == (0b10 << 2))) {};

    // Off HSI.
    RCC->CR &= ~(1 << 0);
    __asm("nop");
    __asm("nop");
    __asm("nop");
    while(RCC->CR & (1 << 1)) {};
}

__attribute__((always_inline)) static inline void wfi () {
    __asm("WFI");
}

void arch_idle() {
    wfi();
}

typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t RES1;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint32_t RES2;
    volatile uint32_t SHPR2;
    volatile uint32_t SHPR3;
} scb_struct; // Doc ID 022979 Rev 1.

// Doc ID 022979 Rev 1, 77/91.
#define SCB      ((scb_struct *)         0xE000ED00)

__attribute__((always_inline)) static inline void dsb () {
    __asm("DSB");
}

void arch_shutdown(arch_shutdown_mode_t mode) {
    switch (mode) {
    case ARCH_SHUTDOWN_MODE_HALT:
    case ARCH_SHUTDOWN_MODE_REBOOT:
    case ARCH_SHUTDOWN_MODE_ABORT:
    default:
        dsb();

        // Doc ID 022979 Rev 1, 80/91.
        SCB->AIRCR = (0x5FA << 16) | (1 << 2);
        break;
    }

    while(1) {
    }
}
