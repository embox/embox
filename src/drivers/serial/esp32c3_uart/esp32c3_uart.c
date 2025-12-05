/**
 * @file
 * @brief
 *
 * @author  Efim Perevalov
 * @date    03.12.2025
 */
#include <assert.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

/* FIFO Configuration */
#define UART_FIFO(base) 	(base + 0x0000) /* RO */
#define UART_MEM_CONF(base) (base + 0x0060) /* R/W */

/* UART Interrupt Register */
#define UART_INT_RAW(base) (base + 0x0004) /* R/WTC/SS */
#define UART_INT_ST(base)  (base + 0x0008) /* RO */
#define UART_INT_ENA(base) (base + 0x000C) /* R/W */
#define UART_INT_CLR(base) (base + 0x0010) /* WT */

/* Configuration Register */
#define UART_CLKDIV(base)       (base + 0x0014) /* R/W */
#define UART_RX_FILT(base)	  	(base + 0x0018) /* R/W */
#define UART_CONF0(base)        (base + 0x0020) /* R/W */
#define UART_CONF1(base)        (base + 0x0024) /* R/W */
#define UART_FLOW_CONF(base)    (base + 0x0034) /* varies */
#define UART_SLEEP_CONF(base) 	(base + 0x0038) /* R/W */
#define UART_SWFC_CONF0(base)   (base + 0x003C) /* R/W */
#define UART_SWFC_CONF1(base)  	(base + 0x0040) /* R/W */
#define UART_TXBRK_CONF(base)  	(base + 0x0044) /* R/W */
#define UART_IDLE_CONF(base)    (base + 0x0048) /* R/W */
#define UART_RS485_CONF(base)   (base + 0x004C) /* R/W */
#define UART_CLK_CONF(base)  		(base + 0x0078) /* R/W */

/* Status Register */
#define UART_STATUS(base)		    (base + 0x001C) /* R/O */
#define UART_MEM_TX_STATUS(base)    (base + 0x0064) /* R/O */
#define UART_MEM_RX_STATUS(base)    (base + 0x0068) /* R/O */
#define UART_FSM_STATUS(base)	    (base + 0x006C) /* R/O */

/* Autobaud Register */
#define UART_LOWPULSE(base)     (base + 0x0028) /* R/O */
#define UART_HIGHPULSE(base)    (base + 0x002C) /* R/O */
#define UART_RXD_CNT(base)      (base + 0x0030) /* R/O */
#define UART_POSPULSE(base)     (base + 0x0070) /* R/O */
#define UART_NEGPULSE(base)     (base + 0x0074) /* R/O */

/* AT Escape Sequence Selection Configuration */
#define UART_AT_CMD_PRECNT(base)    (base + 0x0050) /* R/W */
#define UART_AT_CMD_POSTCNT(base)   (base + 0x0054) /* R/W */
#define UART_AT_CMD_GAPTOUT(base)   ((base) + 0x0058)  /* R/W */
#define UART_AT_CMD_CHAR(base)      ((base) + 0x005C)  /* R/W */

/* Version Register */
#define UART_DATE(base) ((base) + 0x007C)  /* R/W */
#define UART_ID(base)   ((base) + 0x0080)  /* varies */

/* Configuration Register */
#define UHCI_CONF0(base)        ((base) + 0x0000)  /* R/W */
#define UHCI_CONF1(base)        ((base) + 0x0014)  /* varies*/
#define UHCI_ESCAPE_CONF(base)  ((base) + 0x0020)  /* R/W */
#define UHCI_HUNG_CONF(base)    ((base) + 0x0024)  /* R/W */
#define UHCI_ACK_NUM(base)      ((base) + 0x0028)  /* varies */
#define UHCI_QUICK_SENT(base)   ((base) + 0x0030)  /* varies */
#define UHCI_REG_Q0_WORD0(base) ((base) + 0x0034)  /* R/W */
#define UHCI_REG_Q0_WORD1(base) ((base) + 0x0038)  /* R/W */
#define UHCI_REG_Q1_WORD0(base) ((base) + 0x003C)  /* R/W */
#define UHCI_REG_Q1_WORD1(base) ((base) + 0x0040)  /* R/W */
#define UHCI_REG_Q2_WORD0(base) ((base) + 0x0044)  /* R/W */
#define UHCI_REG_Q2_WORD1(base) ((base) + 0x0048)  /* R/W */
#define UHCI_REG_Q3_WORD0(base) ((base) + 0x004C)  /* R/W */
#define UHCI_REG_Q3_WORD1(base) ((base) + 0x0050)  /* R/W */
#define UHCI_REG_Q4_WORD0(base) ((base) + 0x0054)  /* R/W */
#define UHCI_REG_Q4_WORD1(base) ((base) + 0x0058)  /* R/W */
#define UHCI_REG_Q5_WORD0(base) ((base) + 0x005C)  /* R/W */
#define UHCI_REG_Q5_WORD1(base) ((base) + 0x0060)  /* R/W */
#define UHCI_REG_Q6_WORD0(base) ((base) + 0x0064)  /* R/W */
#define UHCI_REG_Q6_WORD1(base) ((base) + 0x0068)  /* R/W */
#define UHCI_ESC_CONF0(base)    ((base) + 0x006C)  /* R/W */
#define UHCI_ESC_CONF1(base)    ((base) + 0x0070)  /* R/W */
#define UHCI_ESC_CONF2(base)    ((base) + 0x0074)  /* R/W */
#define UHCI_ESC_CONF3(base)    ((base) + 0x0078)  /* R/W */
#define UHCI_PKT_THRES(base)    ((base) + 0x007C)  /* R/W */

/* UHCI Interrupt Register */
#define UHCI_INT_RAW(base)  ((base) + 0x0004)  /* varies */
#define UHCI_INT_ST(base)   ((base) + 0x0008)  /* RO */
#define UHCI_INT_ENA(base)  ((base) + 0x000C)  /* R/W */
#define UHCI_INT_CLR(base)  ((base) + 0x0010)  /* WT */

/* HCI Status Register */
#define UHCI_STATE0(base)   ((base) + 0x0018)  /* RO */
#define UHCI_STATE1(base)   ((base) + 0x001C)  /* RO */
#define UHCI_RX_HEAD(base)  ((base) + 0x002c)  /* RO */

/* Version Register */
#define UHCI_DATE(base) ((base) + 0x0080)  /* R/W */
