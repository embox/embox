/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */


#ifndef PLATFORM_ELVEES_ELIOT1_SYSCTL_IOCTR_H_
#define PLATFORM_ELVEES_ELIOT1_SYSCTL_IOCTR_H_

#include <stdint.h>

/**
  * @brief IOCTR (IOCTR)
  */

struct ioctr_regs {                                /*!<  IOCTR Structure                                            */
  volatile uint32_t  PA_MODE;                      /*!< (@ 0x00000000) PA_MODE                                                    */
  volatile uint32_t  PA_AFL;                       /*!< (@ 0x00000004) PA_AFL                                                     */
  volatile uint32_t  PA_AFH;                       /*!< (@ 0x00000008) PA_AFH                                                     */
  volatile uint32_t  PA_PUPD;                      /*!< (@ 0x0000000C) PA_PUPD                                                    */
  volatile uint32_t  PA_DS;                        /*!< (@ 0x00000010) PA_DS                                                      */
  volatile uint32_t  PA_SR;                        /*!< (@ 0x00000014) PA_SR                                                      */
  volatile uint32_t  PA_OTYPE;                     /*!< (@ 0x00000018) PA_OTYPE                                                   */
  volatile uint32_t  PA_ITYPE;                     /*!< (@ 0x0000001C) PA_ITYPE                                                   */
  volatile uint32_t  PA_I2CMODE;                   /*!< (@ 0x00000020) PA_I2CMODE                                                 */
  const    uint32_t  RESERVED[55];
  volatile uint32_t  PB_MODE;                      /*!< (@ 0x00000100) PB_MODE                                                    */
  volatile uint32_t  PB_AFL;                       /*!< (@ 0x00000104) PB_AFL                                                     */
  volatile uint32_t  PB_AFH;                       /*!< (@ 0x00000108) PB_AFH                                                     */
  volatile uint32_t  PB_PUPD;                      /*!< (@ 0x0000010C) PB_PUPD                                                    */
  volatile uint32_t  PB_DS;                        /*!< (@ 0x00000110) PB_DS                                                      */
  volatile uint32_t  PB_SR;                        /*!< (@ 0x00000114) PB_SR                                                      */
  volatile uint32_t  PB_OTYPE;                     /*!< (@ 0x00000118) PB_OTYPE                                                   */
  volatile uint32_t  PB_ITYPE;                     /*!< (@ 0x0000011C) PB_ITYPE                                                   */
  volatile uint32_t  PB_I2CMODE;                   /*!< (@ 0x00000120) PB_I2CMODE                                                 */
  const    uint32_t  RESERVED1[55];
  volatile uint32_t  PC_MODE;                      /*!< (@ 0x00000200) PC_MODE                                                    */
  volatile uint32_t  PC_AFL;                       /*!< (@ 0x00000204) PC_AFL                                                     */
  volatile uint32_t  PC_AFH;                       /*!< (@ 0x00000208) PC_AFH                                                     */
  volatile uint32_t  PC_PUPD;                      /*!< (@ 0x0000020C) PC_PUPD                                                    */
  volatile uint32_t  PC_DS;                        /*!< (@ 0x00000210) PC_DS                                                      */
  volatile uint32_t  PC_SR;                        /*!< (@ 0x00000214) PC_SR                                                      */
  volatile uint32_t  PC_OTYPE;                     /*!< (@ 0x00000218) PC_OTYPE                                                   */
  volatile uint32_t  PC_ITYPE;                     /*!< (@ 0x0000021C) PC_ITYPE                                                   */
  const    uint32_t  RESERVED2[56];
  volatile uint32_t  PD_MODE;                      /*!< (@ 0x00000300) PD_MODE                                                    */
  volatile uint32_t  PD_AFL;                       /*!< (@ 0x00000304) PD_AFL                                                     */
  volatile uint32_t  PD_AFH;                       /*!< (@ 0x00000308) PD_AFH                                                     */
  volatile uint32_t  PD_PUPD;                      /*!< (@ 0x0000030C) PD_PUPD                                                    */
  volatile uint32_t  PD_DS;                        /*!< (@ 0x00000310) PD_DS                                                      */
  volatile uint32_t  PD_SR;                        /*!< (@ 0x00000314) PD_SR                                                      */
  volatile uint32_t  PD_OTYPE;                     /*!< (@ 0x00000318) PD_OTYPE                                                   */
  volatile uint32_t  PD_ITYPE;                     /*!< (@ 0x0000031C) PD_ITYPE                                                   */
};                                   /*!< Size = 800 (0x320)                                                        */


#endif /* PLATFORM_ELVEES_ELIOT1_SYSCTL_IOCTR_H_ */
