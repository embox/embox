//UARTx constant file

#ifndef UART_H
#define UART_H


#include "R1_ccf.h"
#include "gpio.h"

typedef struct
{
  __IO uint32_t DATA;
  __IO uint32_t ST;
  __IO uint32_t CR;
  __IO uint32_t BDR;

} UART_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
#define APB1PERIPH_BASE       (0xC0100000)
//UART0
#define UART0_BASE            (APB0PERIPH_BASE + 0x00000100)
#define UART0                 ((UART_TypeDef *) UART0_BASE)
//UART1
#define UART1_BASE            (APB0PERIPH_BASE + 0x00000200)
#define UART1                 ((UART_TypeDef *) UART1_BASE)
//UART2
#define UART2_BASE            (APB1PERIPH_BASE + 0x00000100)
#define UART2                 ((UART_TypeDef *) UART2_BASE)
//UART3
#define UART3_BASE            (APB1PERIPH_BASE + 0x00000200)
#define UART3                 ((UART_TypeDef *) UART3_BASE)

typedef struct
{
 __IO uint32_t System_freq;
 __IO uint32_t BaudRate;
 __IO uint32_t TypeParity;
 __IO uint32_t Parity;
 __IO uint32_t Mode;
 __IO uint32_t FlowControl;
 __IO uint32_t Loop;
 __IO uint32_t Irq_get_byte;
 __IO uint32_t Irq_send_byte;
 __IO uint32_t Irq_fifo_t;
 __IO uint32_t Irq_fifo_r;
 __IO uint32_t Irq_get_break;
 __IO uint32_t Irq_wait_r;
 __IO uint32_t Irq_shift_empty;

} UART_InitTypeDef;


//uart constant and macro
#define UART_SEND_BYTE(BYTE, UART) UART->DATA = (BYTE) //send byte and select UART number for it
#define UART_FIFO_TX_FULL(UART) ((UART->ST & 0x0200)? 1 : 0) //value fifo_tx_full bit
#define UART_FIFO_TX_EMPTY(UART) ((UART->ST & 0x04)? 1 : 0) //value fifo_tx_empty bit
#define UART_GET_BYTE(UART) UART->DATA //send byte and select UART number for it
#define UART_NEW_DATA(UART) ((UART->ST & 0x01)? 1 : 0) //value uart_new_data bit
#define UART_SHIFT_REG_EMPTY(UART) ((UART->ST & 0x02)? 1 : 0) //value uart_shift_reg_empty bit
#define UART_BREAK_DETECT(UART) ((UART->ST & 0x08)? 1 : 0) //value uart_break_detect bit
#define UART_LOSE_DATA(UART) ((UART->ST & 0x10)? 1 : 0) //value uart_lose_data bit
#define UART_PARITY_ERROR(UART) ((UART->ST & 0x20)? 1 : 0) //value uart_parity_error bit
#define UART_DATAFORM_ERROR(UART) ((UART->ST & 0x40)? 1 : 0) //value uart_dataform_error bit
#define UART_FIFO_TX_FULL_MOREHALF(UART) ((UART->ST & 0x80)? 1 : 0) //value uart_tx_full_morehalf bit
#define UART_FIFO_RX_FULL_MOREHALF(UART) ((UART->ST & 0x100)? 1 : 0) //value uart_rx_full_morehalf bit
#define UART_FIFO_RX_FULL(UART) ((UART->ST & 0x0400)? 1 : 0) //value fifo_rx_full bit
#define UART_READ_CONFIG(UART) UART->CR //read CR register
#define UART_READ_BDR(UART) UART->BDR //read BDR register

//prototypes
void uart_init(UART_TypeDef *UART, UART_InitTypeDef* UART_InitStruct);
void DM2UART(UART_TypeDef *UART, int start_address, int size_block);
void uart_send_str(char *str,UART_TypeDef *UART);
void uart_send_int(int str,UART_TypeDef *UART);
void uart_send_char(unsigned char byte,UART_TypeDef *UART);


#endif
