//USB constant file
#ifndef USB_H
#define USB_H

#include "R1_ccf.h"
#include "gpio.h"

typedef struct
{
  __IO uint32_t USB_En0_CR_OUT;
  __IO uint32_t USB_En0_DMA_CR_OUT;
  __IO uint32_t USB_En0_DMA_ADDR_OUT;
  __IO uint32_t RES0;
  __IO uint32_t USB_En1_CR_OUT;
  __IO uint32_t USB_En1_DMA_CR_OUT;
  __IO uint32_t USB_En1_DMA_ADDR_OUT;
  __IO uint32_t RES1;
  __IO uint32_t USB_En2_CR_OUT;
  __IO uint32_t USB_En2_DMA_CR_OUT;
  __IO uint32_t USB_En2_DMA_ADDR_OUT;
   __IO uint32_t RES2;
  __IO uint32_t USB_En3_CR_OUT;
  __IO uint32_t USB_En3_DMA_CR_OUT;
  __IO uint32_t USB_En3_DMA_ADDR_OUT;
   __IO uint32_t RES3;
  __IO uint32_t USB_En0_CR_IN;
  __IO uint32_t USB_En0_DMA_CR_IN;
  __IO uint32_t USB_En0_DMA_ADDR_IN;
   __IO uint32_t RES4;
  __IO uint32_t USB_En1_CR_IN;
  __IO uint32_t USB_En1_DMA_CR_IN;
  __IO uint32_t USB_En1_DMA_ADDR_IN;
   __IO uint32_t RES5;
  __IO uint32_t USB_En2_CR_IN;
  __IO uint32_t USB_En2_DMA_CR_IN;
  __IO uint32_t USB_En2_DMA_ADDR_IN;
   __IO uint32_t RES6;
  __IO uint32_t USB_En3_CR_IN;
  __IO uint32_t USB_En3_DMA_CR_IN;
  __IO uint32_t USB_En3_DMA_ADDR_IN;
   __IO uint32_t RES7;
  __IO uint32_t RES8;
  __IO uint32_t RES9;
  __IO uint32_t RES10;
  __IO uint32_t RES11;
  __IO uint32_t RES12;
  __IO uint32_t RES13;
  __IO uint32_t RES14;
  __IO uint32_t RES15;
  __IO uint32_t RES16;
  __IO uint32_t RES17;
  __IO uint32_t RES18;
  __IO uint32_t RES19;
  __IO uint32_t RES20;
  __IO uint32_t RES21;
  __IO uint32_t RES22;
  __IO uint32_t RES23;
  __IO uint32_t USB_CR;
  __IO uint32_t USB_ST;

} USB_TypeDef;

#define AHBPERIPH_BASE       (0xFFFF0000)
//USB
#define USB_BASE            (AHBPERIPH_BASE + 0x00000000)
#define USB		         	((USB_TypeDef *) USB_BASE)

//usb constant and macro

#endif
