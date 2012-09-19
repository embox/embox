/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.07.2012
 */

#include <types.h>
#include <unistd.h>

#include <drivers/gpio.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <hal/system.h>

#define RCC_APB1RSTR  0x40021010
#define RCC_APB2ENR   0x40021018
#define RCC_APB2ENR_USART1EN (1 << 14)

struct uart_stm32 {
	uint16_t sr;
	uint16_t reserv0;
	uint16_t dr;
	uint16_t reserv1;
	uint16_t brr;
	uint16_t reserv2;
	uint16_t cr1;
	uint16_t reserv3;
	uint16_t cr2;
	uint16_t reserv4;
	uint16_t cr3;
	uint16_t reserv5;
	uint16_t gtpr;
} __attribute__ ((packed));

#define BAUD_RATE    OPTION_GET(NUMBER,baud_rate)
#if 0
#define HW_FLOW_CTRL OPTION_GET(BOOLEAN,hw_flow)
#else
#define HW_FLOW_CTRL 0
#endif

#define UART1 ((struct uart_stm32 *) 0x40013800)

static struct uart_stm32 * uart1 = UART1;

#define UART_GPIO GPIO_A

#define TX_PIN  (1 << 9)
#define RX_PIN  (1 << 10)
#define CTS_PIN (1 << 11)
#define RTS_PIN (1 << 12)

#define USART_FLAG_RXNE (1 << 5)
#define USART_FLAG_TXE (1 << 7)

#define USART_FLAG_UE (1 << 13)
#define USART_FLAG_RE (1 << 2)
#define USART_FLAG_TE (1 << 3)

#define USART_FLAG_RTSE (1 << 8)
#define USART_FLAG_CTSE (1 << 9)

EMBOX_UNIT_INIT(stm32_usart_init);

void uart_putc(char ch) {
	while (!(REG_LOAD(&uart1->sr) & USART_FLAG_TXE)) { }

	REG_STORE(&uart1->dr, ch);
}

char uart_getc(void) {
	while (!(REG_LOAD(&uart1->sr) & USART_FLAG_RXNE));

	return REG_LOAD(&uart1->dr);
}

static int stm32_usart_init(void) {
	REG_ORIN(RCC_APB2ENR,RCC_APB2ENR_USART1EN);

	gpio_out(UART_GPIO, TX_PIN , GPIO_MODE_OUT_ALTERNATE);
	gpio_in(UART_GPIO, RX_PIN, 0);

#if HW_FLOW_CTRL
	gpio_out(UART_GPIO, RTS_PIN, GPIO_MODE_OUT_ALTERNATE);
	gpio_in(UART_GPIO, CTS_PIN, 0);
	REG_ORIN(&uart1->cr3, USART_FLAG_RTSE | USART_FLAG_CTSE);
#endif

	REG_STORE(&uart1->brr, SYS_CLOCK / BAUD_RATE);
	REG_ORIN(&uart1->cr1, USART_FLAG_RE | USART_FLAG_TE);

	REG_ORIN(&uart1->cr1, USART_FLAG_UE);

	return 0;
}
