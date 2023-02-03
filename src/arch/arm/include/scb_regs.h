/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

#ifndef PLATFORM_ELVEES_ELIOT1_SYSCTL_SYSCTL_H_
#define PLATFORM_ELVEES_ELIOT1_SYSCTL_SYSCTL_H_

#include <stdint.h>

/**
  \brief  Structure type to access the System Control Block (SCB).
 */
struct scb_regs {
	volatile const uint32_t CPUID; /*!< Offset: 0x000 (R/ )  CPUID Base Register */
	volatile uint32_t ICSR; /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
	volatile uint32_t VTOR; /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
	volatile uint32_t AIRCR; /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
	volatile uint32_t SCR; /*!< Offset: 0x010 (R/W)  System Control Register */
	volatile uint32_t CCR; /*!< Offset: 0x014 (R/W)  Configuration Control Register */
	volatile uint8_t SHPR[12U]; /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	volatile uint32_t SHCSR; /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
	volatile uint32_t CFSR; /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
	volatile uint32_t HFSR; /*!< Offset: 0x02C (R/W)  HardFault Status Register */
	volatile uint32_t DFSR; /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
	volatile uint32_t MMFAR; /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
	volatile uint32_t BFAR; /*!< Offset: 0x038 (R/W)  BusFault Address Register */
	volatile uint32_t AFSR; /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
	volatile const uint32_t ID_PFR[2U]; /*!< Offset: 0x040 (R/ )  Processor Feature Register */
	volatile const uint32_t ID_DFR; /*!< Offset: 0x048 (R/ )  Debug Feature Register */
	volatile const uint32_t ID_ADR; /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
	volatile const uint32_t ID_MMFR[4U]; /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
	volatile const uint32_t ID_ISAR[6U]; /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
	volatile const uint32_t CLIDR; /*!< Offset: 0x078 (R/ )  Cache Level ID register */
	volatile const uint32_t CTR; /*!< Offset: 0x07C (R/ )  Cache Type register */
	volatile const uint32_t CCSIDR; /*!< Offset: 0x080 (R/ )  Cache Size ID Register */
	volatile uint32_t CSSELR; /*!< Offset: 0x084 (R/W)  Cache Size Selection Register */
	volatile uint32_t CPACR; /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
	volatile uint32_t NSACR; /*!< Offset: 0x08C (R/W)  Non-Secure Access Control Register */
	uint32_t RESERVED3[92U];
	uint32_t STIR; /*!< Offset: 0x200 ( /W)  Software Triggered Interrupt Register */
	uint32_t RESERVED4[15U];
	volatile const uint32_t MVFR0; /*!< Offset: 0x240 (R/ )  Media and VFP Feature Register 0 */
	volatile const uint32_t MVFR1; /*!< Offset: 0x244 (R/ )  Media and VFP Feature Register 1 */
	volatile const uint32_t MVFR2; /*!< Offset: 0x248 (R/ )  Media and VFP Feature Register 2 */
	uint32_t RESERVED5[1U];
	uint32_t ICIALLU; /*!< Offset: 0x250 ( /W)  I-Cache Invalidate All to PoU */
	uint32_t RESERVED6[1U];
	uint32_t ICIMVAU; /*!< Offset: 0x258 ( /W)  I-Cache Invalidate by MVA to PoU */
	uint32_t DCIMVAC; /*!< Offset: 0x25C ( /W)  D-Cache Invalidate by MVA to PoC */
	uint32_t DCISW; /*!< Offset: 0x260 ( /W)  D-Cache Invalidate by Set-way */
	uint32_t DCCMVAU; /*!< Offset: 0x264 ( /W)  D-Cache Clean by MVA to PoU */
	uint32_t DCCMVAC; /*!< Offset: 0x268 ( /W)  D-Cache Clean by MVA to PoC */
	uint32_t DCCSW; /*!< Offset: 0x26C ( /W)  D-Cache Clean by Set-way */
	uint32_t DCCIMVAC; /*!< Offset: 0x270 ( /W)  D-Cache Clean and Invalidate by MVA to PoC */
	uint32_t DCCISW; /*!< Offset: 0x274 ( /W)  D-Cache Clean and Invalidate by Set-way */
};


#endif /* PLATFORM_ELVEES_ELIOT1_SYSCTL_SYSCTL_H_ */
