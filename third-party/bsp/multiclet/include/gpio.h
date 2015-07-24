//GPIOx constant file
#ifndef GPIO_h
#define GPIO_h

#include "R1_ccf.h"

typedef struct
{
  __IO uint32_t IN;
  __IO uint32_t OUT;
  __IO uint32_t DIR;
  __IO uint32_t MSK;
  __IO uint32_t POL;
  __IO uint32_t EDG;
  __IO uint32_t BPS;

} GPIO_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
#define APB1PERIPH_BASE       (0xC0100000)

//GPIOA
#define GPIOA_BASE            (APB0PERIPH_BASE + 0x000F0000)
#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
//GPIOB
#define GPIOB_BASE            (APB0PERIPH_BASE + 0x000F0100)
#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
//GPIOC
#define GPIOC_BASE            (APB0PERIPH_BASE + 0x000F0200)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)
//GPIOD
#define GPIOD_BASE            (APB1PERIPH_BASE + 0x000F0300)
#define GPIOD                 ((GPIO_TypeDef *) GPIOD_BASE)
//GPIOE
#define GPIOE_BASE            (APB1PERIPH_BASE + 0x000F0400)
#define GPIOE                 ((GPIO_TypeDef *) GPIOE_BASE)
//GPIOF
#define GPIOF_BASE            (APB1PERIPH_BASE + 0x000F0500)
#define GPIOF                 ((GPIO_TypeDef *) GPIOF_BASE)

//gpio constant and macro




#endif
