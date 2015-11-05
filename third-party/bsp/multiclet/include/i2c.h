//I2Cx constant file
#ifndef I2C_H
#define I2C_H

#include "R1_ccf.h"
#include "gpio.h"
//i2c master
typedef struct
{
  __IO uint32_t PSC;
  __IO uint32_t CR;
  __IO uint32_t DATA;
  __IO uint32_t STCMD;

} I2C0_TypeDef;

//i2c slave
typedef struct
{
  __IO uint32_t ADDR;
  __IO uint32_t CR;
  __IO uint32_t ST;
  __IO uint32_t MSK;
  __IO uint32_t RX;
  __IO uint32_t TX;

} I2C1_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)

//I2C0 master
#define I2C0_BASE            (APB0PERIPH_BASE + 0x00001000)
#define I2C0                 ((I2C0_TypeDef *) I2C0_BASE)
//I2C1 slave
#define I2C1_BASE            (APB0PERIPH_BASE + 0x00001100)
#define I2C1                 ((I2C1_TypeDef *) I2C1_BASE)


//constant for i2c0_cmd
#define I2C0_START_WR 0x90 //START + WR
#define I2C0_WR 0x10 //WR
#define I2C0_NACK_RD_STOP 0x68 //NACK + RD + STOP
#define I2C0_STOP_WR 0x50 //STOP + WR

//macro for i2c0

#define I2C0_CMD(CMD) I2C0->STCMD = CMD

#define I2C0_INT_FLAG ((I2C0->STCMD & 0x01)? 1 : 0)

#define I2C0_TIP ((I2C0->STCMD & 0x02)? 1 : 0)

#define I2C0_AL ((I2C0->STCMD & 0x32)? 1 : 0)

#define I2C0_BUSY ((I2C0->STCMD & 0x64)? 1 : 0)

#define I2C0_ACK_RX ((I2C0->STCMD & 0x80)? 1 : 0)

#define I2C0_RX_DATA I2C0->DATA

#define I2C0_TX_DATA(BYTE) I2C0->DATA = BYTE

#define I2C0_EN_INT I2C0->CR |= 0x40

//macro for i2c1

#define I2C1_ADDR(ADDRESS) I2C1->ADDR = ADDRESS

#define I2C1_ST_SEND_NACK ((I2C1->ST & 0x01)? 1 : 0)

#define I2C1_ST_SEND_BYTE ((I2C1->ST & 0x02)? 1 : 0)

#define I2C1_ST_GET_BYTE ((I2C1->ST & 0x04)? 1 : 0)

#define I2C1_CLEAR_SEND_BYTE I2C1->ST |= 2

#define I2C1_CLEAR_SEND_NACK I2C1->ST |= 1

#define I2C1_READ_MSK I2C1->MSK

#define I2C1_READ_CR I2C1->CR

#define I2C1_READ_ADDR I2C1->ADDR

#define I2C1_RX_DATA I2C1->RX

#define I2C1_TX_DATA(BYTE) I2C1->TX = BYTE

//prototypes
//i2c0(master)
void i2c0_init(char enable, int clock_speed, int system_freq);

void i2c0_send(char byte, char slv_addr, int addr);

char i2c0_get(char slv_addr, int addr);

//i2c1(slave)

void i2c1_config(char enable, char transmit_always_valid, char transmit_valid, char tmod, char rmod);

void i2c1_set_msk(char msk_send_nack, char msk_send_byte, char msk_get_byte);


#endif
