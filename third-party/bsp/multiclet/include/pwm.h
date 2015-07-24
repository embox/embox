//PWM constant file
#ifndef PWM_H
#define PWM_H

#include "R1_ccf.h"
#include "gpio.h"


typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t INT;
  __IO uint32_t CNTVAL;
  __IO uint32_t PSC;
  __IO uint32_t CNT;
  __IO uint32_t RESERVED0;
  __IO uint32_t RESERVED1;
  __IO uint32_t RESERVED2;
  __IO uint32_t CMPCH0;
  __IO uint32_t CMPCH1;
  __IO uint32_t CMPCH2;
  __IO uint32_t CMPCH3;

} PWM_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
#define APB1PERIPH_BASE       (0xC0100000)
//PWM
#define PWM_BASE            (APB1PERIPH_BASE + 0x00012000)
#define PWM                 ((PWM_TypeDef *) PWM_BASE)

typedef struct
{
 __IO uint32_t Single_mode_en;
 __IO uint32_t Change_period_en;
 __IO uint32_t Select_mode;
 __IO uint32_t Irq_full_en;
 __IO uint32_t CH0_en;
 __IO uint32_t CH1_en;
 __IO uint32_t CH2_en;
 __IO uint32_t CH3_en;
 __IO uint32_t Set_na_off_lvl_CH0;
 __IO uint32_t Set_na_off_lvl_CH1;
 __IO uint32_t Set_na_off_lvl_CH2;
 __IO uint32_t Set_na_off_lvl_CH3;
 __IO uint32_t Irq_CH0_en;
 __IO uint32_t Irq_CH1_en;
 __IO uint32_t Irq_CH2_en;
 __IO uint32_t Irq_CH3_en;
 __IO uint32_t Set_a_on_lvl_CH0;
 __IO uint32_t Set_a_on_lvl_CH1;
 __IO uint32_t Set_a_on_lvl_CH2;
 __IO uint32_t Set_a_on_lvl_CH3;

} PWM_InitTypeDef;

//macro for pwm
#define PWM_INT_CH(CH_NUM) (PWM->INT & (1 << CH_NUM)? 1 : 0)

//prototypes
void pwm_init(PWM_InitTypeDef* PWM_InitStruct);



#endif
