//Ethernet constant file
#ifndef ETH_H
#define ETH_H

#include "R1_ccf.h"
#include "gpio.h"

typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t ST;
  __IO uint32_t MACMSB;
  __IO uint32_t MACLSB;
  __IO uint32_t MDIO;
  __IO uint32_t TDP;
  __IO uint32_t RDP;
  __IO uint32_t HASHMSB;
  __IO uint32_t HASHLSB;

} ETH_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
#define APB1PERIPH_BASE       (0xC0100000)
//ETH
#define ETH_BASE            (APB0PERIPH_BASE + 0x00005000)
#define ETH		            ((ETH_TypeDef *) ETH_BASE)

typedef struct
{
 __IO uint32_t Transmit_En;
 __IO uint32_t Receive_En;
 __IO uint32_t TX_Int_En;
 __IO uint32_t RX_Int_En;
 __IO uint32_t Full_Duplex;
 __IO uint32_t RX_All_Mode;
 __IO uint32_t Reset;
 __IO uint32_t Speed;
 __IO uint32_t PHY_Int_En;
 __IO uint32_t Multicast_En;

} ETH_InitTypeDef;


#define ETH_ST_RE() ((ETH->ST & 1)? 1 : 0)

#define ETH_ST_TE() ((ETH->ST & 2)? 1 : 0)

#define ETH_ST_RI() ((ETH->ST & 4)? 1 : 0)

#define ETH_ST_TI() ((ETH->ST & 8)? 1 : 0)

#define ETH_ST_RA() ((ETH->ST & 16)? 1 : 0)

#define ETH_ST_TA() ((ETH->ST & 32)? 1 : 0)

#define ETH_ST_TS() ((ETH->ST & 64)? 1 : 0)

#define ETH_ST_IA() ((ETH->ST & 128)? 1 : 0)

#define ETH_ST_PS() ((ETH->ST & 256)? 1 : 0)

#define ETH_CLEAR_ST(ST_NUM) (ETH->ST |= (1 << ST_NUM))


//prototypes
void eth_init(ETH_InitTypeDef* ETH_InitStruct);

#endif
