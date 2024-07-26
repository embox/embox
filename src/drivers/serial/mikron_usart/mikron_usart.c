/**
 * @file

 * @date 01 june  2024
 * @author: Anton Bondarev
 */
#include <stdint.h>

#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#include <drivers/gpio/gpio.h>
#include <drivers/gpio/mikron_gpio.h>

#include <config/board_config.h>

#define SYSFREQ         32000000 /* 32MHz*/
#define USE_BOARD_CONF  OPTION_GET(BOOLEAN, use_bconf)

#if USE_BOARD_CONF
#include "uart_setup_hw_board_config.inc"
#else
static inline int uart_setup_hw(struct uart *dev) {
	gpio_setup_mode(GPIO_PORT_B, 1 << 8, GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(0x1));
	gpio_setup_mode(GPIO_PORT_B, 1 << 9, GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(0x1));
	return 0;
}
#endif /* USE_BOARD_CONF */

#define UART_CONTROL1_M1_S         28
#define UART_CONTROL1_M1_M         (1 << UART_CONTROL1_M1_S)
#define UART_CONTROL1_M0_S         12
#define UART_CONTROL1_M0_M         (1 << UART_CONTROL1_M0_S)
#define UART_CONTROL1_M_8BIT_M     (0)
#define UART_CONTROL1_M_9BIT_M     (UART_CONTROL1_M0_M)
#define UART_CONTROL1_M_7BIT_M     (UART_CONTROL1_M1_M)
#define UART_CONTROL1_M_M          (UART_CONTROL1_M1_M | UART_CONTROL1_M0_M)
#define UART_CONTROL1_M(V)         (((((v) / 10) << UART_CONTROL1_M1_S) | (((v) % 10) << UART_CONTROL1_M0_S)) & UART_CONTROL1_M_M)
#define UART_CONTROL1_PCE_S        10
#define UART_CONTROL1_PCE_M        (1 << UART_CONTROL1_PCE_S)
#define UART_CONTROL1_PS_S         9
#define UART_CONTROL1_PS_M         (1 << UART_CONTROL1_PS_S)
#define UART_CONTROL1_PEIE_S       8
#define UART_CONTROL1_PEIE_M       (1 << UART_CONTROL1_PEIE_S)
#define UART_CONTROL1_TXEIE_S      7
#define UART_CONTROL1_TXEIE_M      (1 << UART_CONTROL1_TXEIE_S)
#define UART_CONTROL1_TCIE_S       6
#define UART_CONTROL1_TCIE_M       (1 << UART_CONTROL1_TCIE_S)
#define UART_CONTROL1_RXNEIE_S     5
#define UART_CONTROL1_RXNEIE_M     (1 << UART_CONTROL1_RXNEIE_S)
#define UART_CONTROL1_IDLEIE_S     4
#define UART_CONTROL1_IDLEIE_M     (1 << UART_CONTROL1_IDLEIE_S)
#define UART_CONTROL1_TE_S         3
#define UART_CONTROL1_TE_M         (1 << UART_CONTROL1_TE_S)
#define UART_CONTROL1_RE_S         2
#define UART_CONTROL1_RE_M         (1 << UART_CONTROL1_RE_S)
#define UART_CONTROL1_UE_S         0
#define UART_CONTROL1_UE_M         (1 << UART_CONTROL1_UE_S)

#define UART_CONTROL2_MSBFIRST_S   19
#define UART_CONTROL2_MSBFIRST_M   (1 << UART_CONTROL2_MSBFIRST_S)
#define UART_CONTROL2_DATAINV_S    18
#define UART_CONTROL2_DATAINV_M    (1 << UART_CONTROL2_DATAINV_S)
#define UART_CONTROL2_TXINV_S      17
#define UART_CONTROL2_TXINV_M      (1 << UART_CONTROL2_TXINV_S)
#define UART_CONTROL2_RXINV_S      16
#define UART_CONTROL2_RXINV_M      (1 << UART_CONTROL2_RXINV_S)
#define UART_CONTROL2_SWAP_S       15
#define UART_CONTROL2_SWAP_M       (1 << UART_CONTROL2_SWAP_S)
#define UART_CONTROL2_LBM_S        14
#define UART_CONTROL2_LBM_M        (1 << UART_CONTROL2_LBM_S)
#define UART_CONTROL2_STOP_1_S     13
#define UART_CONTROL2_STOP_1_M     (1 << UART_CONTROL2_STOP_1_S)
#define UART_CONTROL2_CLKEN_S      11
#define UART_CONTROL2_CLKEN_M      (1 << UART_CONTROL2_CLKEN_S)
#define UART_CONTROL2_CPOL_S       10
#define UART_CONTROL2_CPOL_M       (1 << UART_CONTROL2_CPOL_S)
#define UART_CONTROL2_CPHA_S       9
#define UART_CONTROL2_CPHA_M       (1 << UART_CONTROL2_CPHA_S)
#define UART_CONTROL2_LBCL_S       8
#define UART_CONTROL2_LBCL_M       (1 << UART_CONTROL2_LBCL_S)
#define UART_CONTROL2_LBDIE_S      6    
#define UART_CONTROL2_LBDIE_M      (1 << UART_CONTROL2_LBDIE_S)

#define UART_CONTROL3_OVRDIS_S     12
#define UART_CONTROL3_OVRDIS_M     (1 << UART_CONTROL3_OVRDIS_S)
#define UART_CONTROL3_CTSIE_S      10
#define UART_CONTROL3_CTSIE_M      (1 << UART_CONTROL3_CTSIE_S)
#define UART_CONTROL3_CTSE_S       9
#define UART_CONTROL3_CTSE_M       (1 << UART_CONTROL3_CTSE_S)
#define UART_CONTROL3_RTSE_S       8
#define UART_CONTROL3_RTSE_M       (1 << UART_CONTROL3_RTSE_S)
#define UART_CONTROL3_DMAT_S       7
#define UART_CONTROL3_DMAT_M       (1 << UART_CONTROL3_DMAT_S)
#define UART_CONTROL3_DMAR_S       6
#define UART_CONTROL3_DMAR_M       (1 << UART_CONTROL3_DMAR_S)
#define UART_CONTROL3_HDSEL_S      3
#define UART_CONTROL3_HDSEL_M      (1 << UART_CONTROL3_HDSEL_S)
#define UART_CONTROL3_BKRQ_S       2
#define UART_CONTROL3_BKRQ_M       (1 << UART_CONTROL3_BKRQ_S)
// #define UART_CONTROL3_IREN_S       1
// #define UART_CONTROL3_IREN_M       (1 << UART_CONTROL3_IREN_S)
#define UART_CONTROL3_EIE_S        0
#define UART_CONTROL3_EIE_M        (1 << UART_CONTROL3_EIE_S)

#define UART_DIVIDER_BRR_S        0
#define UART_DIVIDER_BRR_M        (0xFFFF << UART_DIVIDER_BRR_S)

#define UART_FLAGS_REACK_S         22
#define UART_FLAGS_REACK_M         (1 << UART_FLAGS_REACK_S)
#define UART_FLAGS_TEACK_S         21
#define UART_FLAGS_TEACK_M         (1 << UART_FLAGS_TEACK_S)
#define UART_FLAGS_BUSY_S          16
#define UART_FLAGS_BUSY_M          (1 << UART_FLAGS_BUSY_S)
#define UART_FLAGS_CTS_S           10
#define UART_FLAGS_CTS_M           (1 << UART_FLAGS_CTS_S)
#define UART_FLAGS_CTSIF_S         9
#define UART_FLAGS_CTSIF_M         (1 << UART_FLAGS_CTSIF_S)
#define UART_FLAGS_LBDF_S          8
#define UART_FLAGS_LBDF_M          (1 << UART_FLAGS_LBDF_S)
#define UART_FLAGS_TXE_S           7
#define UART_FLAGS_TXE_M           (1 << UART_FLAGS_TXE_S)
#define UART_FLAGS_TC_S            6
#define UART_FLAGS_TC_M            (1 << UART_FLAGS_TC_S)
#define UART_FLAGS_RXNE_S          5
#define UART_FLAGS_RXNE_M          (1 << UART_FLAGS_RXNE_S)
#define UART_FLAGS_IDLE_S          4
#define UART_FLAGS_IDLE_M          (1 << UART_FLAGS_IDLE_S)
#define UART_FLAGS_ORE_S           3
#define UART_FLAGS_ORE_M           (1 << UART_FLAGS_ORE_S)
#define UART_FLAGS_NF_S            2
#define UART_FLAGS_NF_M            (1 << UART_FLAGS_NF_S)
#define UART_FLAGS_FE_S            1
#define UART_FLAGS_FE_M            (1 << UART_FLAGS_FE_S)
#define UART_FLAGS_PE_S            0
#define UART_FLAGS_PE_M            (1 << UART_FLAGS_PE_S)

#define UART_MODEM_DTR_S           8
#define UART_MODEM_DTR_M           (1 << UART_MODEM_DTR_S)
#define UART_MODEM_DCD_S           7
#define UART_MODEM_DCD_M           (1 << UART_MODEM_DCD_S)
#define UART_MODEM_RI_S            6
#define UART_MODEM_RI_M            (1 << UART_MODEM_RI_S)
#define UART_MODEM_DSR_S           5
#define UART_MODEM_DSR_M           (1 << UART_MODEM_DSR_S)
#define UART_MODEM_DCDIF_S         3
#define UART_MODEM_DCDIF_M         (1 << UART_MODEM_DCDIF_S)
#define UART_MODEM_RIIF_S          2
#define UART_MODEM_RIIF_M          (1 << UART_MODEM_RIIF_S)
#define UART_MODEM_DSRIF_S         1
#define UART_MODEM_DSRIF_M         (1 << UART_MODEM_DSRIF_S)

struct mikron_uart_regs {
	volatile uint32_t CONTROL1;
	volatile uint32_t CONTROL2;
	volatile uint32_t CONTROL3;
	volatile uint32_t DIVIDER;
	volatile uint32_t _reserved0;
	volatile uint32_t _reserved1;
	volatile uint32_t _reserved2;
	volatile uint32_t FLAGS;
	volatile uint32_t _reserved3;
	volatile uint32_t RXDATA;
	volatile uint32_t TXDATA;
	volatile uint32_t MODEM;
};

static void mikron_usart_set_baudrate(struct uart *dev) {
	uint32_t divider = SYSFREQ / dev->params.baud_rate;
	struct mikron_uart_regs *regs;

	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	REG32_STORE(&regs->DIVIDER, divider); 
}

static int mikron_usart_irq_enable(struct uart *dev, 
									const struct uart_params *params) {
	struct mikron_uart_regs *regs;

	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG32_ORIN(&regs->CONTROL1, UART_CONTROL1_RXNEIE_M);
	}

	return 0;
}

static int mikron_usart_irq_disable(struct uart *dev,
									const struct uart_params *params) {
	struct mikron_uart_regs *regs;

	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	REG32_ANDIN(&regs->CONTROL1, ~UART_CONTROL1_RXNEIE_M);

	return 0;
}

static int mikron_usart_setup(struct uart *dev, 
								const struct uart_params *params) {
	uint32_t ctrl;
	uint32_t flags;
	struct mikron_uart_regs *regs;

	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	ctrl = UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_M_8BIT_M;
	if (params->uart_param_flags & UART_PARAM_FLAGS_8BIT_WORD) {
		ctrl |= UART_CONTROL1_M_8BIT_M;		
	}
	ctrl |= UART_CONTROL1_UE_M;

	/* Disable uart. */
	REG32_STORE(&regs->CONTROL1, 0);
	REG32_STORE(&regs->CONTROL2, 0);
	REG32_STORE(&regs->CONTROL3, 0);
	REG32_STORE(&regs->FLAGS, 0xFFFFFFFF);

	uart_setup_hw(dev);
	
	mikron_usart_set_baudrate(dev);

	REG32_STORE(&regs->CONTROL1, ctrl);

	mikron_usart_irq_enable(dev, params);

	flags = 0;
	do {
		flags = REG32_LOAD(&regs->FLAGS);
	} while(!(flags & UART_FLAGS_REACK_M) && !(flags | UART_FLAGS_TEACK_M));

	return 0;
}

static int mikron_usart_putc(struct uart *dev, int ch) {
	struct mikron_uart_regs *regs;
	
	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	while ( 0 == (REG_LOAD(&regs->FLAGS) & UART_FLAGS_TXE_M)) {
	}

	REG32_STORE(&regs->TXDATA, ch);

	return 0;
}

static int mikron_usart_has_symbol(struct uart *dev) {
	struct mikron_uart_regs *regs;

	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	return (REG_LOAD(&regs->FLAGS) & UART_FLAGS_RXNE_M) ? 1 :0 ;
}

static int mikron_usart_getc(struct uart *dev) {
	struct mikron_uart_regs *regs;

	regs = (struct mikron_uart_regs *)(uintptr_t) dev->base_addr;

	return REG_LOAD(&regs->RXDATA) & 0xFF;
}

const struct uart_ops mikron_usart_ops = {
    .uart_getc = mikron_usart_getc,
    .uart_putc = mikron_usart_putc,
    .uart_hasrx = mikron_usart_has_symbol,
    .uart_setup = mikron_usart_setup,
    .uart_irq_en = mikron_usart_irq_enable,
    .uart_irq_dis = mikron_usart_irq_disable,
};
