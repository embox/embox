/**
 * @file fpioa.h
 * @brief
 * @author sksat <sksat@sksat.net>
 * @version 0.1
 * @date 2020-08-15
 */

#ifndef SRC_DRIVERS_GPIO_K210_FPIOA_H_
#define SRC_DRIVERS_GPIO_K210_FPIOA_H_

#define SYSCTL_BASE_ADDR    (0x50440000U)
#define FPIOA_BASE_ADDR     (0x502B0000U)

typedef struct {
    uint32_t cpu      : 1;
    uint32_t sram0    : 1;
    uint32_t sram1    : 1;
    uint32_t apb0     : 1;
    uint32_t apb1     : 1;
    uint32_t apb2     : 1;
    uint32_t reserved : 26;
} __attribute__ ((packed)) sysctl_clock_enable_central;

typedef struct {
    uint32_t rom      : 1;
    uint32_t dma      : 1;
    uint32_t ai       : 1;
    uint32_t dvp      : 1;
    uint32_t fft      : 1;
    uint32_t gpio     : 1;
    uint32_t spi0     : 1;
    uint32_t spi1     : 1;
    uint32_t spi2     : 1;
    uint32_t spi3     : 1;
    uint32_t i2s0     : 1;
    uint32_t i2s1     : 1;
    uint32_t i2s2     : 1;
    uint32_t i2c0     : 1;
    uint32_t i2c1     : 1;
    uint32_t i2c2     : 1;
    uint32_t uart1    : 1;
    uint32_t uart2    : 1;
    uint32_t uart3    : 1;
    uint32_t aes      : 1;
    uint32_t fpioa    : 1;
    uint32_t timer0   : 1;
    uint32_t timer1   : 1;
    uint32_t timer2   : 1;
    uint32_t wdt0     : 1;
    uint32_t wdt1     : 1;
    uint32_t sha      : 1;
    uint32_t otp      : 1;
    uint32_t reserved0: 1;
    uint32_t rtc      : 1;
    uint32_t reserved1: 2;
} __attribute__((packed)) sysctl_clock_enable_peripheral;

static int k210_fpioa_init(void);

#endif
