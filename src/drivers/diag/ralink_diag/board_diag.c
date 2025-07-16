/**
 * @file
 * @brief
 *
 * @date 04.12.2024
 * @author Serge Vasilugin
 */

#include <stddef.h>
#include <string.h>
#include <framework/mod/options.h>
#include <drivers/diag.h>

#define UART_BASE_ADDR  OPTION_GET(NUMBER,base_addr)
#define BAUD_RATE OPTION_GET(NUMBER,baud_rate)


#define READREG(r)		*(volatile unsigned int *)(r)
#define WRITEREG(r,v)		*(volatile unsigned int *)(r) = v

#define KSEG1ADDR(_x)		(((_x) & 0x1fffffff) | 0xa0000000)

#ifdef UART_BASE_ADDR
#define	UART_BASE KSEG1ADDR(UART_BASE_ADDR)
#else

#ifdef SOC_RT288X
#define UART_BASE		0xb0300c00
#else
#define UART_BASE		0xb0000c00
#endif

#endif

#define UART_RX			0
#define UART_TX			1<<2
#define UART_LSR		7<<2

#define UART_LSR_DR		1
#define UART_LSR_THRE		0x20

#define UART_READ(r)		READREG(UART_BASE + (r))
#define UART_WRITE(r,v)		WRITEREG(UART_BASE + (r), (v))

static int board_init(const struct diag *diag)
{
	return 0;
}

static void board_putc(const struct diag *diag, char ch)
{
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
	UART_WRITE(UART_TX, ch);
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
}

static int board_kbhit(const struct diag *diag)
{
	return UART_READ(UART_LSR) & UART_LSR_DR;
}

static char board_getc(const struct diag *diag)
{
	return UART_READ(UART_RX);
}

DIAG_OPS_DEF(
	.init = board_init,
	.putc = board_putc,
	.getc = board_getc,
	.kbhit = board_kbhit,
);
