/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

#ifndef PLATFORM_ELVEES_ELIOT1_ARCH_CLKCTR_H_
#define PLATFORM_ELVEES_ELIOT1_ARCH_CLKCTR_H_

#include <stdint.h>


struct clkctr_regs {                                /*!< (@ 0x40031000) CLKCTR Structure                                           */
  volatile uint32_t  PLLCFG;                       /*!< (@ 0x00000000) PLLCFG                                                     */
  volatile uint32_t  PLLDIAG;                      /*!< (@ 0x00000004) PLLDIAG                                                    */
  volatile uint32_t  CFG;                          /*!< (@ 0x00000008) CFG                                                        */
  volatile uint32_t  CLKFORCE;                     /*!< (@ 0x0000000C) CLKFORCE                                                   */
  volatile uint32_t  FCLKDIV;                      /*!< (@ 0x00000010) FCLKDIV                                                    */
  volatile uint32_t  SYSCLKDIV;                    /*!< (@ 0x00000014) SYSCLKDIV                                                  */
  volatile uint32_t  QSPICLKDIV;                   /*!< (@ 0x00000018) QSPICLKDIV                                                 */
  volatile uint32_t  I2SCLKDIV;                    /*!< (@ 0x0000001C) I2SCLKDIV                                                  */
  volatile uint32_t  MCODIV;                       /*!< (@ 0x00000020) MCODIV                                                     */
  volatile uint32_t  GNSSCLKDIV;                   /*!< (@ 0x00000024) GNSSCLKDIV                                                 */
  volatile uint32_t  RESERVED[2];
  volatile uint32_t  HFITRIM;                      /*!< (@ 0x00000030) HFITRIM                                                    */
};                                  /*!< Size = 52 (0x34)                                                          */

extern void clkctr_set_pll(struct clkctr_regs * base, uint32_t xti_hz, uint16_t pll_mul);
extern void clkctr_set_sys_div(struct clkctr_regs * base, uint16_t fclk_div,
	    uint16_t sysclk_div, uint16_t gnssclk_div, uint16_t qspiclk_div);


#endif /* PLATFORM_ELVEES_ELIOT1_ARCH_CLKCTR_H_ */
