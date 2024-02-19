/**
 * @file
 *
 * @date 13.03.10
 * @author Eldar Abusalimov
 */

#include <stdint.h>
#include <errno.h>

#include <drivers/common/memory.h>

#include <drivers/amba_pnp.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>

#include <drivers/diag.h>
#include <drivers/serial/uart_dev.h>
#include <drivers/serial/diag_serial.h>

#include <embox/unit.h>

#include <kernel/printk.h>

#define BAUD_RATE \
	OPTION_GET(NUMBER, baud_rate)

#define UART_SCALER_VAL \
	(((SYS_CLOCK * 10) / (BAUD_RATE * 8) - 5) / 10)

/* status register bit masks */
#define UART_STAT_DR (1u << 0) /**< Data is available for read in RX register*/
#define UART_STAT_TS (1u << 1) /**< TX shift register is empty */
#define UART_STAT_TE (1u << 2) /**< TX FIFO is empty */
#define UART_STAT_BR (1u << 3) /**< BREAK received */
#define UART_STAT_OV (1u << 4) /**< 1 or more bytes lossed over overflow */
#define UART_STAT_PE (1u << 5) /**< error in parity control */
#define UART_STAT_FE (1u << 6) /**< error in frame */
#define UART_STAT_TH (1u << 7) /**< TX FIFO is half-full */
#define UART_STAT_RH (1u << 8) /**< RX FIFO is half-full */
#define UART_STAT_TF (1u << 9) /**< TX FIFO is full */
#define UART_STAT_RF (1u << 10)/**< RX FIFO is full */

/* control register bit masks */
#define UART_CTRL_RE (1u << 0)  /**< receiver enable */
#define UART_CTRL_TE (1u << 1)  /**< transmitter enable */
#define UART_CTRL_RI (1u << 2)  /**< interrupt after receiving frame */
#define UART_CTRL_TI (1u << 3)  /**< interrupt after transmitting frame */
#define UART_CTRL_PS (1u << 4)  /**< parity check (0 - oddness, 1 - evenness) */
#define UART_CTRL_PE (1u << 5)  /**< parity control enable */
#define UART_CTRL_FL (1u << 6)  /**< flow control with CTS/RTS enable (don't use) */
#define UART_CTRL_LB (1u << 7)  /**< loopback enable */
#define UART_CTRL_EC (1u << 8)  /**< external clock enable */
#define UART_CTRL_TF (1u << 9)  /**< enable interrupt on FIFO transmitter layer */
#define UART_CTRL_RF (1u << 10) /**< enable interrupt on FIFO receiver layer */
#define UART_CTRL_DB (1u << 11) /**< debug mode enable (don't available) */
#define UART_CTRL_OE (1u << 12) /**< output transmitter enable */
#define UART_CTRL_FA (1u << 31) /**< set on when FIFO TX and RX are available */
#define UART_DISABLE_ALL 0

struct apbuart_regs {
	/*------+------+
	 | resv | data |
	 | 31-8 | 7-0  |
	 +------+------*/
	/* 0x0 */uint32_t data;
	/*-------+-------+-------+--+--+--+--+--+--+--+--+--+--+--+
	 | RCNT  | TCNT  | resrv |RF|TF|RH|TH|FE|PE|OV|BR|TE|TS|DR|
	 | 31-26 | 25-20 | 19-11 |10|9 |8 |7 |6 |5 |4 |3 |2 |1 |0 |
	 +-------+-------+-------+--+--+--+--+--+--+--+--+--+--+--*/
	/* 0x4 */uint32_t status;
	/*--+-------+--+--+--+--+--+--+--+--+--+--+--+--+--+
	 |FA| resrv |OE|DB|RF|TF|EC|LB|FL|PE|PS|TI|RI|TE|RE|
	 |31| 30-13 |12|11|10|9 |8 |7 |6 |5 |4 |3 |2 |1 |0 |
	 +--+-------+--+--+--+--+--+--+--+--+--+--+--+--+--*/
	/* 0x8 */uint32_t ctrl;
	/*-------+--------+
	 | resrv | scaler |
	 | 31-12 | 11-0   |
	 +-------+--------*/
	/* 0xC */uint32_t scaler;
};

static volatile struct apbuart_regs *dev_regs;

static int dev_regs_init(void);

EMBOX_UNIT_INIT(uart_init);

static int apbuart_setup(struct uart *dev, const struct uart_params *params) {
	int res;

	if (NULL == dev_regs && 0 != (res = dev_regs_init())) {
		return res;
	}

	assert(NULL != dev_regs);

	REG_STORE(&dev_regs->ctrl, UART_DISABLE_ALL);
	REG_STORE(&dev_regs->scaler, UART_SCALER_VAL);
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG_STORE(&dev_regs->ctrl, UART_CTRL_TE | UART_CTRL_RE | UART_CTRL_RI);
	} else {
		REG_STORE(&dev_regs->ctrl, UART_CTRL_TE | UART_CTRL_RE);
	}

	return 0;
}

static int apbuart_irq_en(struct uart *dev, const struct uart_params *params) {

	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG_STORE(&dev_regs->ctrl, UART_CTRL_TE | UART_CTRL_RE | UART_CTRL_RI);
	}

	return 0;
}

static int apbuart_irq_dis(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		REG_STORE(&dev_regs->ctrl, UART_CTRL_TE | UART_CTRL_RE);
	}

	return 0;
}

static int apbuart_putc(struct uart *dev, int ch) {
	while (!(UART_STAT_TE & REG_LOAD(&dev_regs->status))) {
	}
	REG_STORE(&dev_regs->data, (uint32_t) ch);

	return 0;
}

static int apbuart_has_symbol(struct uart *dev) {
	return (UART_STAT_DR & REG_LOAD(&dev_regs->status));
}

static int apbuart_getc(struct uart *dev) {
	return REG_LOAD(&dev_regs->data);
}


#ifdef DRIVER_AMBAPP
static int dev_regs_init() {
	amba_dev_t amba_dev;
	if (-1 == capture_amba_dev(&amba_dev, AMBAPP_VENDOR_GAISLER,
					AMBAPP_DEVICE_GAISLER_APBUART, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
				AMBAPP_VENDOR_GAISLER, AMBAPP_DEVICE_GAISLER_APBUART);
		return -ENODEV;
	}
	dev_regs = (volatile struct apbuart_regs *) amba_dev.bar[0].start;
	return 0;
}
#elif OPTION_DEFINED(NUMBER,apbuart_base)

PERIPH_MEMORY_DEFINE(apbuart, OPTION_GET(NUMBER,apbuart_base), sizeof(struct apbuart_regs));

static int dev_regs_init() {
	dev_regs = (volatile struct apbuart_regs *) OPTION_GET(NUMBER,apbuart_base);
	return 0;
}
#else
# error "Either DRIVER_AMBAPP or apbuart_base option must be defined"
#endif /* DRIVER_AMBAPP */

static const struct uart_ops uart_ops = {
		.uart_getc = apbuart_getc,
		.uart_putc = apbuart_putc,
		.uart_hasrx = apbuart_has_symbol,
		.uart_setup = apbuart_setup,
		.uart_irq_en = apbuart_irq_en,
		.uart_irq_dis = apbuart_irq_dis,
};

static struct uart uart0 = {
		.irq_num = OPTION_GET(NUMBER,irq_num),
		.base_addr = OPTION_GET(NUMBER,apbuart_base),
		.uart_ops = &uart_ops,
};

static const struct uart_params uart_defparams = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD | UART_PARAM_FLAGS_USE_IRQ,
};

static const struct uart_params uart_diag_params = {
		.baud_rate = OPTION_GET(NUMBER,baud_rate),
		.uart_param_flags = UART_PARAM_FLAGS_8BIT_WORD,
};

DIAG_SERIAL_DEF(&uart0, &uart_diag_params);

static int uart_init(void) {
	return uart_register(&uart0, &uart_defparams);
}

