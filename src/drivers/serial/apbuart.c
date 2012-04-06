/**
 * @file
 *
 * @date 13.03.10
 * @author Eldar Abusalimov
 */

#include <types.h>
#include <errno.h>

#include <kernel/irq.h>
#include <hal/reg.h>
#include <drivers/amba_pnp.h>
#include <kernel/printk.h>
#include <embox/device.h>
#include <fs/file.h>

#include <embox/unit.h>

#include <module/embox/arch/system.h>
#define SYS_CLOCK     OPTION_MODULE_GET(embox__arch__system,NUMBER,core_freq)

#define UART_SCALER_VAL \
	((((SYS_CLOCK * 10) / (8 * OPTION_GET(NUMBER,baud_rate))) - 5) / 10)

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


static int uart_init(void) {
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

static void uart_putc(char ch) {
	while (!(UART_STAT_TE & REG_LOAD(&dev_regs->status))) {
	}
	REG_STORE(&dev_regs->data, (uint32_t) ch);
}

static char uart_getc(void) {
	while (!(UART_STAT_DR & REG_LOAD(&dev_regs->status))) {
	}
	return ((char) REG_LOAD(&dev_regs->data));
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
	irq_num = amba_dev.dev_info.irq;
	return 0;
}
#elif OPTION_DEFINED(NUMBER,apbuart_base)
static int dev_regs_init() {
	dev_regs = (volatile struct apbuart_regs *) OPTION_GET(NUMBER,apbuart_base);
	irq_num = CONFIG_APBUART_IRQ;
	return 0;
}
#else
# error "Either DRIVER_AMBAPP or apbuart_base option must be defined"
#endif /* DRIVER_AMBAPP */



/* ADD_CHAR_DEVICE(TTY1,uart_getc,uart_getc); */

static void *apb_open(struct file_desc *desc);
static int apb_close(struct file_desc *desc);
static size_t apb_read(void *buf, size_t size, size_t count, void *file);
static size_t apb_write(const void *buff, size_t size, size_t count, void *file);

static file_operations_t file_op = {
		.fread = apb_read,
		.fopen = apb_open,
		.fclose = apb_close,
		.fwrite = apb_write
};

/*
 * file_operation
 */
static void *apb_open(struct file_desc *desc) {
	if(NULL == dev_regs) {
		uart_init();
	}
	desc->ops = &file_op;
	return (void *) desc;
}

static int apb_close(struct file_desc *desc) {
	return 0;
}

static size_t apb_read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;

	int i = count * size;

	while (i --) {
		*(ch_buf++) = uart_getc();
	}

	return 0;
}

static size_t apb_write(const void *buff, size_t size, size_t count, void *file) {
	size_t cnt = 0;
	char *b = (char*) buff;

	while (cnt != count * size) {
		uart_putc(b[cnt++]);
	}
	return 0;
}


EMBOX_DEVICE("uart", &file_op);
