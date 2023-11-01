/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.23
 */
#ifndef DRIVERS_INTERRUPT_GIC_GICV3_H_
#define DRIVERS_INTERRUPT_GIC_GICV3_H_

#include "gic_base.h"
#include "gicv2.h"

/* clang-format off */
#define GICD_TYPER2     (GICR_BASE + 0x000C) /* Interrupt controller Type Register 2 */
#define GICD_STATUSR    (GICR_BASE + 0x0010) /* Error Reporting Status Register, optional */
#define GICD_SETSPI_NSR (GICR_BASE + 0x0040) /* Set SPI Register */
#define GICD_CLRSPI_NSR (GICR_BASE + 0x0048) /* Clear SPI Register */
#define GICD_SETSPI_SR  (GICR_BASE + 0x0050) /* Set SPI, Secure Register */
#define GICD_CLRSPI_SR  (GICR_BASE + 0x0058) /* Clear SPI, Secure Register */

#define GICR_CTLR      (GICR_BASE + 0x0000) /* Redistributor Control Register */
#define GICR_IIDR      (GICR_BASE + 0x0004) /* Implementer Identification Register */
#define GICR_TYPER     (GICR_BASE + 0x0008) /* Redistributor Type Register */
#define GICR_STATUSR   (GICR_BASE + 0x0010) /* RError Reporting Status Register */
#define GICR_WAKER     (GICR_BASE + 0x0014) /* Redistributor Wake Register */
#define GICR_MPAMIDR   (GICR_BASE + 0x0018) /* Report maximum PARTID and PMG Register */
#define GICR_PARTIDR   (GICR_BASE + 0x001C) /* Set PARTID and PMG Register */
#define GICR_PROPBASER (GICR_BASE + 0x0070) /* Redistributor Properties Base Address Register */
#define GICR_PENDBASER (GICR_BASE + 0x0078) /* Redistributor LPI Pending Table Base Address Register */

#define GICR_IGROUPR0      (GICR_BASE + 0x0080) /* Interrupt Group Register 0 */
#define GICR_ISENABLER0    (GICR_BASE + 0x0100) /* Interrupt Set-Enable Register 0 */
#define GICR_ICENABLER0    (GICR_BASE + 0x0180) /* Interrupt Clear-Enable Register 0 */
#define GICR_ISPENDR0      (GICR_BASE + 0x0200) /* Interrupt Set-Pend Register 0 */
#define GICR_ICPENDR0      (GICR_BASE + 0x0280) /* Interrupt Clear-Pend Register 0 */
#define GICR_ISACTIVER0    (GICR_BASE + 0x0300) /* Interrupt Set-Active Register 0 */
#define GICR_ICACTIVER0    (GICR_BASE + 0x0380) /* Interrupt Clear-Active Register 0 */
#define GICR_IPRIORITYR(n) (GICR_BASE + 0x0400) /* Interrupt Priority Registers */
#define GICR_ICFGR0        (GICR_BASE + 0x0C00) /* SGI Configuration Register */
#define GICR_ICFGR1        (GICR_BASE + 0x0C04) /* PPI Configuration Register */
#define GICR_IGRPMODR0     (GICR_BASE + 0x0D00) /* Interrupt Group Modifier Register 0 */
#define GICR_NSACR         (GICR_BASE + 0x0E00) /* Non-Secure Access Control Register */
/* clang-format on */

/* GICD_CTLR */
#define GICD_CTLR_GRP0    (1U << 0) /* Enable Group 0 interrupts */
#define GICD_CTLR_GRP1_NS (1U << 1) /* Enable Non-secure Group 1 interrupts */
#define GICD_CTLR_GRP1_S  (1U << 2) /* Enable Secure Group 1 interrupts */
#define GICD_CTLR_ARE_S   (1U << 4) /* Affinity Routing Enable, Secure state */
#define GICD_CTLR_ARE_NS \
	(1U << 5) /* Affinity Routing Enable, Non-secure state */

/* GICR_WAKER */
#define GICR_WAKER_PS \
	(1U << 1) /* Indicates whether the Redistributor can assert the WakeRequest signal */
#define GICR_WAKER_CA \
	(1U << 2) /* Indicates whether the connected PE is quiescent */

/* ICC_SRE */
#define ICC_SRE_EN  (1U << 0) /* System Register Enable */
#define ICC_SRE_DFB (1U << 1) /* Disable FIQ bypass */
#define ICC_SRE_DIB (1U << 1) /* Disable IRQ bypass */

/* ICC_CTLR_EL1 */
#define ICC_CTLR_EL1_EOIM (1U << 1) /* EOI mode for the current Security state */

#endif /* DRIVERS_INTERRUPT_GIC_GICV3_H_ */
