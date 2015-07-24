//WDTx constant file
#ifndef WDT_H
#define WDT_H

#include "R1_ccf.h"
#include "gpio.h"

typedef struct
{
  __IO uint32_t CNT;
  __IO uint32_t KEY;
  __IO uint32_t RESERVED0;
  __IO uint32_t ST;

} WDT_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
//WDT
#define WDT_BASE            (APB0PERIPH_BASE + 0x000E0000)
#define WDT		            ((WDT_TypeDef *) WDT_BASE)

//constant for wdt
#define WDT_ON (WDT->KEY = 0x5555) //wdt enable
#define WDT_OFF (WDT->KEY = 0x3333) //wdt disable
#define WDT_RESET (WDT->KEY = 0xAAAA) //wdt reset
#define WDT_SET_CNT_EN (WDT->KEY = 0xCCCC) //wdt allow set cnt
#define WDT_READ_CNT (WDT->CNT) //read cnt register
#define WDT_SET_CNT(PERIOD) (WDT->CNT = PERIOD) //set period
#define WDT_READ_ST (WDT->ST) //get status register



//prototypes
//wdt
void wdt_set_cycle(int period);
#endif
