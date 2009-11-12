/**
 * \file uart.c
 */
#include "types.h"
#include "kernel/irq.h"
#include "cpu_conf.h"
#include "conio.h"
#include "drivers/amba_pnp.h"
#include "kernel/uart.h"
#include "common.h"

#define UART_RX_READY     (1 << 0)
#define UART_TX_READY     (1 << 2) /*< hold register empty */

/* status register bit masks */
#define UART_STAT_DR      (1 << 0) /*< Data is available for read in RX holding register*/
#define UART_STAT_TS      (1 << 1) /*< TX shift register is empty */
#define UART_STAT_TE      (1 << 2) /*< TX FIFO is emoty */
#define UART_STAT_BR      (1 << 3) /*< BREAK received */
#define UART_STAT_OV      (1 << 4) /*< 1 or more bytes lossed over overflow */
#define UART_STAT_PE      (1 << 5) /*< error in parity control */
#define UART_STAT_FE      (1 << 6) /*< error in frame */
#define UART_STAT_TH      (1 << 7) /*< TX FIFO is half-full */
#define UART_STAT_RH      (1 << 8) /*< RX FIFO is half-full */
#define UART_STAT_TF      (1 << 9) /*< TX FIFO is full */
#define UART_STAT_RF      (1 << 10)/*< RX FIFO is full */

/* control register bit masks */
#define UART_DISABLE_ALL  0x00000000 /*< disable all */
#define UART_CTRL_RE      (1 << 0)  /*< receiver enable */
#define UART_CTRL_TE      (1 << 1)  /*< transmitter enable */
#define UART_CTRL_RI      (1 << 2)  /*< enable interrupt after receiving frame */
#define UART_CTRL_TI      (1 << 3)  /*< enable interrupt after transmitting frame */
#define UART_CTRL_PS      (1 << 4)  /*< type of parity control (0 - check for oddness, 1 - check for evenness) */
#define UART_CTRL_PE      (1 << 5)  /*< parity control enable */
#define UART_CTRL_FL      (1 << 6)  /*< flow control with CTS/RTS enable (don't use) */
#define UART_CTRL_LB      (1 << 7)  /*< loopback enable */
#define UART_CTRL_EC      (1 << 8)  /*< external clock enable */
#define UART_CTRL_TF      (1 << 9)  /*< enable interrupt on FIFO transmitter layer */
#define UART_CTRL_RF      (1 << 10) /*< enable interrupt on FIFO receiver layer */
#define UART_CTRL_DB      (1 << 11) /*< debug mode enable (don't available) */
#define UART_CTRL_OE      (1 << 12) /*< output transmitter enable */
#define UART_CTRL_FA      (1 << 31) /*< set on when FIFO TX and RX are available */

typedef struct _UART_STRUCT {
	/** _____________
	 * |31_____8|7__0|
	 * |RESERVED|DATA|
	 */
	volatile UINT32 data; /* 0x0 */
	/** _____________________________________________________
	 * |31_26|25_20|19____11|10|9_|8_|7_|6_|5_|4_|3_|2_|1_|0_|
	 * |RCNT |TCNT |RESERVED|RF|TF|RH|TH|FE|PE|OV|BR|TE|TS|DR|
	 */
	volatile UINT32 status; /* 0x4 */
	/** __________________________________________________
	 * |31|30____13|12|11|10|9_|8_|7_|6_|5_|4_|3_|2_|1_|0_|
	 * |FA|RESERVED|OE|DB|RF|TF|EC|LB|FL|PE|PS|TI|RI|TE|RE|
	 */
	volatile UINT32 ctrl; /* 0x8 */
	/** ____________________________
	 * |31____12|11________________0|
	 * |RESERVED|SCALER RELOAD VALUE|
	 */
	volatile UINT32 scaler; /* 0xC */
} UART_STRUCT;

static volatile UART_STRUCT *dev_regs = NULL;

#include "drivers/amba_drivers_helper.h"
#undef module_init
#define module_init() uart_init()

static int irq;

int uart_init() {
	if (NULL != dev_regs)
		return -1;

	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_UART);

	if (UART_BASE != amba_dev.bar[0].start) {
		TRACE ("uart base is %x instead of correct value %x\n", amba_dev.bar[0].start, UART_BASE);
		return -1;
	}
	if (UART_IRQ != amba_dev.dev_info.irq) {
		TRACE ("uart irq is %d instead of correct value %d", amba_dev.dev_info.irq, UART_IRQ);
		return -1;
	}

	dev_regs = (UART_STRUCT *) amba_dev.bar[0].start;
	irq = amba_dev.dev_info.irq;
	REG_STORE(dev_regs->ctrl, UART_DISABLE_ALL); /**< disabled uart */
	REG_STORE(dev_regs->scaler, UART_SCALER_VAL);
	REG_STORE(dev_regs->ctrl, UART_CTRL_TE | UART_CTRL_RE); /**< enable uart */
#ifndef SIMULATION_TRG
	//while (!(UART_TX_READY & REG_LOAD(dev_regs->status)));
	//clear uart
	//while (UART_RX_READY & REG_LOAD(dev_regs->status));
	//uart->data;
#endif
}

/*BOOL uart_is_empty() {
 return (UART_RX_READY & REG_LOAD(uart->status)) ? FALSE : TRUE;
 }*/

void uart_putc(char ch) {
	ASSERT_MODULE_INIT();
	/*#ifndef SIMULATION_TRG
	 while (!(UART_TX_READY & REG_LOAD(dev_regs->status)))
	 ;
	 #endif*/
	volatile int i;
	for (i = 0; i < 0x1000; i++)
		;

	REG_STORE(dev_regs->data, (UINT32) ch);
}

char uart_getc() {
	ASSERT_MODULE_INIT ();

	while (!(UART_RX_READY & REG_LOAD(dev_regs->status)))
		;

	return ((char) REG_LOAD(dev_regs->data));
}

static IRQ_INFO irq_info;
static BOOL handler_was_set = FALSE;

int uart_set_irq_handler(IRQ_HANDLER pfunc) {
	ASSERT_MODULE_INIT ();

	REG_ORIN(dev_regs->ctrl, UART_CTRL_RI);

	irq_info.enabled = TRUE;
	irq_info.irq_num = irq;
	irq_info.handler = pfunc;
	irq_set_info(&irq_info);
	handler_was_set = TRUE;
	//irq_set_handler(irq, pfunc);
	return 0;
}

int uart_remove_irq_handler() {
	ASSERT_MODULE_INIT ();

	REG_ANDIN(dev_regs->ctrl, ~UART_CTRL_RI);
	if (handler_was_set) {
		irq_set_info(&irq_info);
		handler_was_set = FALSE;
	}
	//irq_set_handler(irq, NULL);
	return 0;
}

