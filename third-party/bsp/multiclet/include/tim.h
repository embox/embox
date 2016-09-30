//TIMx constant file
#ifndef TIM_H
#define TIM_H

#include "R1_ccf.h"
#include "gpio.h"

typedef struct
{
  __IO uint32_t PSCVAL;
  __IO uint32_t PSCPER;
  __IO uint32_t RESERVED0;
  __IO uint32_t RESERVED1;
  __IO uint32_t CNTVAL0;
  __IO uint32_t CNT0;
  __IO uint32_t CR0;
  __IO uint32_t RESERVED2;
  __IO uint32_t CNTVAL1;
  __IO uint32_t CNT1;
  __IO uint32_t CR1;
  __IO uint32_t RESERVED3;
  __IO uint32_t CNTVAL2;
  __IO uint32_t CNT2;
  __IO uint32_t CR2;
  __IO uint32_t RESERVED4;
    __IO uint32_t CNTVAL3;
  __IO uint32_t CNT3;
  __IO uint32_t CR3;
  __IO uint32_t RESERVED5;

} TIM_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
#define APB1PERIPH_BASE       (0xC0100000)
//Timer 0
#define TIM0_BASE            (APB0PERIPH_BASE + 0x00010000)
#define TIM0		         ((TIM_TypeDef *) TIM0_BASE)

//Timer 1
#define TIM1_BASE            (APB0PERIPH_BASE + 0x00010100)
#define TIM1		         ((TIM_TypeDef *) TIM1_BASE)

//Timer 2
#define TIM2_BASE            (APB1PERIPH_BASE + 0x00010000)
#define TIM2		         ((TIM_TypeDef *) TIM2_BASE)

//Timer 3
#define TIM3_BASE            (APB1PERIPH_BASE + 0x00010100)
#define TIM3		         ((TIM_TypeDef *) TIM3_BASE)


//timer constant and macro
#define TIM_CR(EN, MODE, LOAD, IE, INT, TIM, CH) TIM->CR##CH = ((EN & 0x01)|((MODE & 0x01)<<1)|((LOAD & 0x01)<<2)|((IE & 0x01)<<3)|((INT & 0x01)<<4))
#define TIM_PSCVAL(PSCVALUE, TIM) TIM->PSCVAL = PSCVALUE
#define TIM_PSCPER(PSCPERIOD, TIM) TIM->PSCPER = PSCPERIOD
#define TIM_CNTVAL(CNTVALUE, TIM, CH) TIM->CNTVAL##CH = CNTVALUE
#define TIM_CNT(VALUE, TIM, CH) TIM->CNT##CH = VALUE
#define TIM_CLEAR_INT(TIM, CH) TIM->CNT##CH |= 0x10

#endif
