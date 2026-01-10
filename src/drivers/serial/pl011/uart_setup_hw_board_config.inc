#include <config/board_config.h>

#include <drivers/gpio.h>

extern int clk_enable(char *clk_name);

static inline int uart_nr_by_addr(uintptr_t base_addr) {
	int num;

	switch (base_addr) {
#if defined CONF_UART0_ENABLED
	case CONF_UART0_REGION_BASE_ADDR:
		num = 0;
		break;
#endif
#if defined CONF_UART1_ENABLED
	case CONF_UART1_REGION_BASE_ADDR:
		num = 1;
		break;
#endif
#if defined CONF_UART2_ENABLED
	case CONF_UART2_REGION_BASE_ADDR:
		num = 2;
		break;
#endif
#if defined CONF_UART3_ENABLED
	case CONF_UART3_REGION_BASE_ADDR:
		num = 3;
		break;
#endif
#if defined CONF_UART4_ENABLED
	case CONF_UART4_REGION_BASE_ADDR:
		num = 4;
		break;
#endif
	default:
		return -1;
	}
	return num;
}

static inline int uart_get_tx_port(int num) {
	int port = -1;

	switch (num) {
#if defined CONF_UART0_PIN_TX_PORT
	case 0:
		port = CONF_UART0_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART1_PIN_TX_PORT
	case 1:
		port = CONF_UART1_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART2_PIN_TX_PORT
	case 2:
		port = CONF_UART2_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART3_PIN_TX_PORT
	case 3:
		port = CONF_UART3_PIN_TX_PORT;
		break;
#endif
#if defined CONF_UART4_PIN_TX_PORT
	case 4:
		port = CONF_UART4_PIN_TX_PORT;
		break;
#endif
	default:
		port = -1;
	}

	return port;
}

static inline int uart_get_tx_pin(int num) {
	int pin = -1;

	switch (num) {
#if defined CONF_UART0_PIN_TX_NR
	case 0:
		pin = CONF_UART0_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART1_PIN_TX_NR
	case 1:
		pin = CONF_UART1_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART2_PIN_TX_NR
	case 2:
		pin = CONF_UART2_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART3_PIN_TX_NR
	case 3:
		pin = CONF_UART3_PIN_TX_NR;
		break;
#endif
#if defined CONF_UART4_PIN_TX_NR
	case 4:
		pin = CONF_UART4_PIN_TX_NR;
		break;
#endif
	default:
		pin = -1;
	}

	return pin;
}

static inline int uart_get_tx_alt(int num) {
	int alt = -1;

	switch (num) {
#if defined CONF_UART0_PIN_TX_AF
	case 0:
		alt = CONF_UART0_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART1_PIN_TX_AF
	case 1:
		alt = CONF_UART1_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART2_PIN_TX_AF
	case 2:
		alt = CONF_UART2_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART3_PIN_TX_AF
	case 3:
		alt = CONF_UART3_PIN_TX_AF;
		break;
#endif
#if defined CONF_UART4_PIN_TX_AF
	case 4:
		alt = CONF_UART4_PIN_TX_AF;
		break;
#endif
	default:
		alt = -1;
	}

	return alt;
}

static inline int uart_get_rx_port(int num) {
	int port = -1;

	switch (num) {
#if defined CONF_UART0_PIN_RX_PORT
	case 0:
		port = CONF_UART0_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART1_PIN_RX_PORT
	case 1:
		port = CONF_UART1_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART2_PIN_RX_PORT
	case 2:
		port = CONF_UART2_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART3_PIN_RX_PORT
	case 3:
		port = CONF_UART3_PIN_RX_PORT;
		break;
#endif
#if defined CONF_UART4_PIN_RX_PORT
	case 4:
		port = CONF_UART4_PIN_RX_PORT;
		break;
#endif
	default:
		port = -1;
	}

	return port;
}

static inline int uart_get_rx_pin(int num) {
	int pin = -1;

	switch (num) {
#if defined CONF_UART0_PIN_RX_NR
	case 0:
		pin = CONF_UART0_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART1_PIN_RX_NR
	case 1:
		pin = CONF_UART1_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART2_PIN_RX_NR
	case 2:
		pin = CONF_UART2_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART3_PIN_RX_NR
	case 3:
		pin = CONF_UART3_PIN_RX_NR;
		break;
#endif
#if defined CONF_UART4_PIN_RX_NR
	case 4:
		pin = CONF_UART4_PIN_RX_NR;
		break;
#endif
	default:
		pin = -1;
	}

	return pin;
}

static inline int uart_get_rx_alt(int num) {
	int alt = -1;

	switch (num) {
#if defined CONF_UART0_PIN_RX_AF
	case 0:
		alt = CONF_UART0_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART1_PIN_RX_AF
	case 1:
		alt = CONF_UART1_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART2_PIN_RX_AF
	case 2:
		alt = CONF_UART2_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART3_PIN_RX_AF
	case 3:
		alt = CONF_UART3_PIN_RX_AF;
		break;
#endif
#if defined CONF_UART4_PIN_RX_AF
	case 4:
		alt = CONF_UART4_PIN_RX_AF;
		break;
#endif
	default:
		alt = -1;
	}

	return alt;
}

static inline void uart_set_pins(int num) {
	gpio_setup_mode(uart_get_tx_port(num),
			(1 << uart_get_tx_pin(num)),
			GPIO_MODE_OUT | GPIO_MODE_ALT_SET(uart_get_tx_alt(num)));

	gpio_setup_mode(uart_get_rx_port(num),
			(1 << uart_get_rx_pin(num)),
			GPIO_MODE_IN | GPIO_MODE_ALT_SET(uart_get_rx_alt(num)));
}

static inline int uart_set_clk(int num) {
	char *clk_name;
	switch (num) {
#if defined CONF_UART0_CLK_ENABLE
		case 0:
		clk_name = CONF_UART0_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART1_CLK_ENABLE
		case 1:
		clk_name = CONF_UART1_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART2_CLK_ENABLE
		case 2:
		clk_name = CONF_UART2_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART3_CLK_ENABLE
		case 3:
		clk_name = CONF_UART3_CLK_ENABLE();
		break;
#endif
#if defined CONF_UART4_CLK_ENABLE
		case 4:
		clk_name = CONF_UART4_CLK_ENABLE();
		break;
#endif
	default:
		return -1;
	}

	clk_enable(clk_name);

	return  0;
}

static inline int uart_setup_hw(struct uart *dev) {
	int uart_num = uart_nr_by_addr(dev->base_addr);

	uart_set_pins(uart_num);
	uart_set_clk(uart_num);

	return 0;
}
