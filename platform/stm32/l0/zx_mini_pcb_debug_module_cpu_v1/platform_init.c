/**
 * @file
 * @brief
 *
 * @author  Vadim Deryabkin
 * @date    07.02.2020
 */

#include <assert.h>
#include <stdint.h>

#include <hal/platform.h>
#include <hal/system.h>

typedef struct {
	volatile uint32_t CR;        // 0x00
	volatile uint32_t ICSCR;     // 0x04
	volatile uint32_t RES_1;     // 0x08
	volatile uint32_t CFGR;      // 0x0C
	volatile uint32_t CIER;      // 0x10
	volatile uint32_t CIFR;      // 0x14
	volatile uint32_t CICR;      // 0x18
	volatile uint32_t IOPRSTR;   // 0x1C
	volatile uint32_t AHBRSTR;   // 0x20
	volatile uint32_t APB2RSTR;  // 0x24
	volatile uint32_t APB1RSTR;  // 0x28
	volatile uint32_t IOPENR;    // 0x2C
	volatile uint32_t AHBENR;    // 0x30
	volatile uint32_t APB2ENR;   // 0x34
	volatile uint32_t APB1ENR;   // 0x38
	volatile uint32_t IOPSMEN;   // 0x3C
	volatile uint32_t AHBSMENR;  // 0x40
	volatile uint32_t APB2SMENR; // 0x44
	volatile uint32_t APB1SMENR; // 0x48
	volatile uint32_t CCIPR;     // 0x4C
	volatile uint32_t CSR;       // 0x50
} rcc_struct;                    // Doc: DocID031151 Rev 1, RM0451, 185/774.

typedef struct {
	volatile uint32_t ACR;       // 0x00
	volatile uint32_t PECR;      // 0x04
	volatile uint32_t PDKEYR;    // 0x08
	volatile uint32_t PKEYR;     // 0x0C
	volatile uint32_t PRGKEYR;   // 0x10
	volatile uint32_t OPTKEYR;   // 0x14
	volatile uint32_t SR;        // 0x18
	volatile uint32_t OPTR;      // 0x1C
	volatile uint32_t WRPROT1;   // 0x20
	volatile uint32_t RES_1[23]; // 0x24 - 0x7C
	volatile uint32_t WRPROT2;   // 0x80
} flash_struct;                  // Doc: DocID031151 Rev 1, RM0451, 93/774.

// Doc: DocID031151 Rev 1, RM0451, 39/774.
#define RCC   ((rcc_struct *)0X40021000)
#define FLASH ((flash_struct *)0X40022000)

// HSE is not present.
const uint32_t HSE_VALUE = 0;

void platform_init(void) {
	// Board is always working in max frequency.
	static_assert(SYS_CLOCK == 32000000, "");

	// On HSI (16 MHz).
	RCC->CR |= (1 << 0);
	while (!(RCC->CR & (1 << 2))) {};

	// PLL_CLK = HSI * 6 / 3 == 32 MHz
	RCC->CFGR = (0b0010 << 18) | (0b10 << 22);

	// On PLL.
	RCC->CR |= (1 << 24);
	while (!(RCC->CR & (1 << 25))) {};

	// Set latency.
	FLASH->ACR = (1 << 0);

	// Set CPU_CLK = PLL.
	RCC->CFGR |= (0b11 << 0);
	while (!((RCC->CFGR & (0b11 << 2)) == (0b11 << 2))) {};

	// Off MSI.
	RCC->CR &= ~(1 << 8);
	while (RCC->CR & (1 << 9)) {};

	// On prft, pd, pre.
	FLASH->ACR |= (1 << 1) | (1 << 4) | (1 << 6);
}
