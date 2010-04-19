/**
 * \file uart.c
 */

#include <types.h>
#include <errno.h>

#include <kernel/irq.h>
#include <hal/reg.h>
#include <drivers/amba_pnp.h>
#include <kernel/printk.h>

#define UART_SCALER_VAL \
	((((CONFIG_CORE_FREQ*10) / (8 * CONFIG_UART_BAUD_RATE))-5)/10)

/* status register bit masks */
#define UART_STAT_DR (1 << 0) /**< Data is available for read in RX register*/
#define UART_STAT_TS (1 << 1) /**< TX shift register is empty */
#define UART_STAT_TE (1 << 2) /**< TX FIFO is empty */
#define UART_STAT_BR (1 << 3) /**< BREAK received */
#define UART_STAT_OV (1 << 4) /**< 1 or more bytes lossed over overflow */
#define UART_STAT_PE (1 << 5) /**< error in parity control */
#define UART_STAT_FE (1 << 6) /**< error in frame */
#define UART_STAT_TH (1 << 7) /**< TX FIFO is half-full */
#define UART_STAT_RH (1 << 8) /**< RX FIFO is half-full */
#define UART_STAT_TF (1 << 9) /**< TX FIFO is full */
#define UART_STAT_RF (1 << 10)/**< RX FIFO is full */

/* control register bit masks */
#define UART_CTRL_RE (1 << 0)  /**< receiver enable */
#define UART_CTRL_TE (1 << 1)  /**< transmitter enable */
#define UART_CTRL_RI (1 << 2)  /**< interrupt after receiving frame */
#define UART_CTRL_TI (1 << 3)  /**< interrupt after transmitting frame */
#define UART_CTRL_PS (1 << 4)  /**< parity check (0 - oddness, 1 - evenness) */
#define UART_CTRL_PE (1 << 5)  /**< parity control enable */
#define UART_CTRL_FL (1 << 6)  /**< flow control with CTS/RTS enable (don't use) */
#define UART_CTRL_LB (1 << 7)  /**< loopback enable */
#define UART_CTRL_EC (1 << 8)  /**< external clock enable */
#define UART_CTRL_TF (1 << 9)  /**< enable interrupt on FIFO transmitter layer */
#define UART_CTRL_RF (1 << 10) /**< enable interrupt on FIFO receiver layer */
#define UART_CTRL_DB (1 << 11) /**< debug mode enable (don't available) */
#define UART_CTRL_OE (1 << 12) /**< output transmitter enable */
#define UART_CTRL_FA (1 << 31) /**< set on when FIFO TX and RX are available */
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
static irq_nr_t irq_num;

int uart_init(void) {
	int err;

	if (NULL != dev_regs) {
		return -EBUSY;
	}

	if (0 != (err = dev_regs_init())) {
		return err;
	}
	assert(NULL != dev_regs);

	REG_STORE(&dev_regs->ctrl, UART_DISABLE_ALL);
	REG_STORE(&dev_regs->scaler, UART_SCALER_VAL);
	REG_STORE(&dev_regs->ctrl, UART_CTRL_TE | UART_CTRL_RE);

	return 0;
}

void uart_putc(char ch) {
	volatile int i;
	for (i = 0; i < 0x1000; i++) {
	}
	REG_STORE(&dev_regs->data, (uint32_t) ch);
}

char uart_getc(void) {
	while (!(UART_STAT_DR & REG_LOAD(&dev_regs->status))) {
	}
	return ((char) REG_LOAD(&dev_regs->data));
}

#ifdef CONFIG_AMBAPP
static int dev_regs_init() {
	AMBA_DEV amba_dev;
	if (-1 == capture_amba_dev(&amba_dev, CONFIG_VENDOR_ID_GAISLER,
					CONFIG_DEV_ID_GAISLER_UART, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
				CONFIG_VENDOR_ID_GAISLER, CONFIG_DEV_ID_GAISLER_UART);
		return -ENODEV;
	}
	dev_regs = (volatile struct apbuart_regs *) amba_dev.bar[0].start;
	irq_num = amba_dev.dev_info.irq;
	return 0;
}
#elif defined(CONFIG_APBUART_BASE)
static int dev_regs_init() {
	dev_regs = (volatile struct apbuart_regs *) CONFIG_APBUART_BASE;
	irq_num = CONFIG_APBUART_IRQ;
	return 0;
}
#else
# error "Either CONFIG_AMBAPP or CONFIG_APBUART_BASE must be defined"
#endif /* CONFIG_AMBAPP */

#if 1

static bool handler_was_set = false;

int uart_set_irq_handler(irq_handler_t pfunc) {
//	ASSERT_INIT_DONE ();

	REG_ORIN((&dev_regs->ctrl), UART_CTRL_RI);

	irq_attach(irq_num, pfunc,0,NULL,"uart");
	handler_was_set = true;
	//irq_set_handler(irq, pfunc);
	return 0;
}

int uart_remove_irq_handler(void) {
//	ASSERT_INIT_DONE ();

	REG_ANDIN((&dev_regs->ctrl), ~UART_CTRL_RI);
	if (handler_was_set) {
		irq_detach(irq_num, NULL);
		handler_was_set = false;
	}
	//irq_set_handler(irq, NULL);
	return 0;
}

#endif

