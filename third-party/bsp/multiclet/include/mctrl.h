//Memory Controller constant file
#ifndef MCTRL_H
#define MCTRL_H

typedef struct
{
  __IO uint32_t MCFG1;
  __IO uint32_t MCFG2;
  __IO uint32_t MCFG3;
  __IO uint32_t MCFG4;

} MCTRL_TypeDef;

#define APB0PERIPH_BASE       (0xC0000000)
//MCTRL
#define MCTRL_BASE          (APB0PERIPH_BASE + 0x000EE000)
#define MCTRL		        ((MCTRL_TypeDef *) MCTRL_BASE)


#endif
