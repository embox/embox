#include <embox/unit.h>
#include <kernel/driver.h>
#include <kernel/printk.h>
#include <stdio.h>

#define UART_DIAG_REALIZATION
#if 0
# define DEBUG_FI_UART
#endif

#ifdef UART_DIAG_REALIZATION
/**
 * @file
 * @brief TODO
 *
 * @date 14.02.2010
 * @author Eldar Abusalimov
 */

#include <types.h>

#include <kernel/diag.h>
#include <hal/reg.h>

#define APBUART_BASE  0x80000100
#define DATA_REG             0x0
#define STATUS_REG           0x4
#define CTRL_REG             0x8
#define SCALER_REG           0xc

#define SCALER_VAL \
	((((CONFIG_CORE_FREQ*10) / (8 * CONFIG_UART_BAUD_RATE))-5)/10)

typedef struct uart_private {
	int ioctl_base_flags;
} uart_private_t;

void fi_uart_init(void) {
	REG_STORE((volatile uint32_t *) (APBUART_BASE + SCALER_REG), SCALER_VAL);
	REG_STORE((volatile uint32_t *) (APBUART_BASE + CTRL_REG), 0x3);
}

char fi_uart_getc(void) {
	while (!(0x1 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)))) {
	}
	return ((char) REG_LOAD((volatile uint32_t *) (APBUART_BASE + DATA_REG)));
}

int fi_uart_has_symbol(void) {
	return (0x1 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)));
}

void fi_uart_putc(char ch) {
#ifdef DEBUG_FI_UART
	TRACE("fi_uart_putc %d %c\n",(int)ch, ch);
#endif
	while (!(0x4 & REG_LOAD((volatile uint32_t *) (APBUART_BASE + STATUS_REG)))) {
	}
	REG_STORE((volatile uint32_t *) (APBUART_BASE + DATA_REG), (uint32_t) ch);
}

#else /* UART_DIAG_REALIZATION */
#error MODIFY IT /* non realized non-blocked i/o */
/**
 * @file fi_fi_uart.c (file inteface)
 */

#if 0
#include <types.h>
#include <errno.h>

#include <kernel/irq.h>
#include <hal/reg.h>
#include <drivers/amba_pnp.h>
#include <kernel/printk.h>
#include <dev/char_device.h>

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

struct apbfi_uart_regs {
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

static volatile struct apbfi_uart_regs *dev_regs;

static int dev_regs_init(void);
static irq_nr_t irq_num;

int fi_uart_init(void) {
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

void fi_uart_putc(char ch) {
	while (!(UART_STAT_TE & REG_LOAD(&dev_regs->status))) {
	}
	REG_STORE(&dev_regs->data, (uint32_t) ch);
}

char fi_uart_getc(void) {
	while (!(UART_STAT_DR & REG_LOAD(&dev_regs->status))) {
	}
	return ((char) REG_LOAD(&dev_regs->data));
}

#ifdef CONFIG_AMBAPP
static int dev_regs_init(void) {
	amba_dev_t amba_dev;
	if (-1 == capture_amba_dev(&amba_dev, CONFIG_VENDOR_ID_GAISLER,
					CONFIG_DEV_ID_GAISLER_UART, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
				CONFIG_VENDOR_ID_GAISLER, CONFIG_DEV_ID_GAISLER_UART);
		return -ENODEV;
	}
	dev_regs = (volatile struct apbfi_uart_regs *) amba_dev.bar[0].start;
	irq_num = amba_dev.dev_info.irq;
	return 0;
}
#elif defined(CONFIG_APBUART_BASE)
static int dev_regs_init(void) {
	dev_regs = (volatile struct apbfi_uart_regs *) CONFIG_APBUART_BASE;
	irq_num = CONFIG_APBUART_IRQ;
	return 0;
}
#else
# error "Either CONFIG_AMBAPP or CONFIG_APBUART_BASE must be defined"
#endif /* CONFIG_AMBAPP */


static bool handler_was_set = false;

int fi_uart_set_irq_handler(irq_handler_t pfunc) {
	REG_ORIN((&dev_regs->ctrl), UART_CTRL_RI);

	irq_attach(irq_num, pfunc,0,NULL,"fi_uart");
	handler_was_set = true;
	return 0;
}

int fi_uart_remove_irq_handler(void) {
	REG_ANDIN((&dev_regs->ctrl), ~UART_CTRL_RI);
	if (handler_was_set) {
		irq_detach(irq_num, NULL);
		handler_was_set = false;
	}

	return 0;
}

/* ADD_CHAR_DEVICE(TTY1,fi_uart_getc,fi_uart_getc); */
#endif
#endif /* UART_DIAG_REALIZATION */

/* shared interface */

/**
 * @author Fedor Burdun
 */

#define START_AS_MOD

/*
 * interface for IO
 */
int fi_uart_open(device_t *dev, int mode) {
	return 0;
}

int fi_uart_close(device_t *dev) {
	return 0;
}

int fi_uart_read(device_t *dev, char *buf, size_t n) {
	int i;
	if (((uart_private_t*)(dev->private))->ioctl_base_flags & IOCTLP_NONBLOCKINGIO) {
		for (i=0 ; (i<n)&&(fi_uart_has_symbol()) ; ++i) {
			buf[i] = (char) fi_uart_getc();
		}
		return i;
	} else {
		for (i=0 ; i<n ; ++i) {
			buf[i] = (char) fi_uart_getc();
		}
		return 0;
	}
}

int fi_uart_write(device_t *dev, char *buf, size_t n) {
	int i;
	if (((uart_private_t*)(dev->private))->ioctl_base_flags & IOCTLP_NONBLOCKINGIO) {
		for (i=0 ; (i<n) ; ++i) {
			fi_uart_putc(buf[i]);
		}
		return i;
	} else {
		for (i = 0 ; i < n ; ++i) {
			fi_uart_putc(buf[i]);
		}
		return 0;
	}
}

int fi_uart_ioctl(device_t *dev, io_cmd c, void *arg) {
	/* printk("device: %ld, cmd: %d, arg: %ld\n",  dev, c , *(int*) arg); */
	switch (c) {
	case IOCTL_SET_BASE_OPTIONS:
		((uart_private_t*)(dev->private))->ioctl_base_flags = *((int*) arg);
		break;
	case IOCTL_GET_BASE_OPTIONS:
		arg = &(((uart_private_t*)(dev->private))->ioctl_base_flags);
		break;
	default:
		return IOCTLR_UNKNOW_OPERATION;
	}
	return IOCTLR_OK;
}

int fi_uart_devctl(device_t *dev, device_cmd c, void *arg) {
	return 0;
}

/*
 * interface for registry in embox as driver
 */
int fi_uart_load(driver_t *drv) {
	drv->name       = "Gaisler Uart";
	drv->ops.open   = fi_uart_open;
	drv->ops.close  = fi_uart_close;
	drv->ops.read   = fi_uart_read;
	drv->ops.write  = fi_uart_write;
	drv->ops.ioctl  = fi_uart_ioctl;
	drv->ops.devctl = fi_uart_devctl;
	drv->flags      = 0;
	drv->private    = NULL;
	drv->private_s  = sizeof(device_t*);
	return 0;
}

int fi_uart_probe(driver_t *drv , void *arg) {
	drv->private = device_create(drv, "dev_fi_uart01", 0, sizeof(uart_private_t));
	return 0;
}

int fi_uart_unload(driver_t *drv) {
	device_destroy(drv->private);
	drv->private = NULL;
	drv->private_s = 0;
	return 0;
}

/*
 * interface for registry in embox as module (while don't exist driver's framework)
 */
#ifdef START_AS_MOD
/*
 * for work need add to mods-? mods(?.fi_uart, 1) or ?
 */
driver_t *drv;
/* driver_t drv_wm; // without malloc */

static int fi_uart_start(void) {
	TRACE("\e[1;34mGaisler fi_uart driver was started!\e[0;0m\n");
	if (NULL == (drv = kmalloc(sizeof(driver_t)))) {
		TRACE("No memory enough for start gaisler fi_uart driver\n");
		return 1;
	}
	fi_uart_load(drv);
	fi_uart_probe(drv, NULL);
	return 0;
}

static int fi_uart_stop(void) {
	fi_uart_unload(drv);
	kfree(drv);
	return 0;
}

EMBOX_UNIT(fi_uart_start, fi_uart_stop);

#else

EMBOX_DEVICE(fi_uart_load, fi_uart_probe, fi_uart_unload);

#endif

