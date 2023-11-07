/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.23
 */
#ifndef DRIVERS_INTERRUPT_GIC_GICV1_H_
#define DRIVERS_INTERRUPT_GIC_GICV1_H_

#include "gic_base.h"

/* clang-format off */
#define GICD_CTLR                (GICD_BASE + 0x000)           /* Distributor Control Register */
#define GICD_TYPER               (GICD_BASE + 0x004)           /* Interrupt Controller Type Register */
#define GICD_IIDR                (GICD_BASE + 0x008)           /* Distributor Implementer Identification Register */
#define GICD_IGROUPR(n)          (GICD_BASE + 0x080 + 4 * (n)) /* Interrupt Group Registers */
#define GICD_ISENABLER(n)        (GICD_BASE + 0x100 + 4 * (n)) /* Interrupt Set-Enable Registers */
#define GICD_ICENABLER(n)        (GICD_BASE + 0x180 + 4 * (n)) /* Interrupt Clear-Enable Registers */
#define GICD_ISPENDR(n)          (GICD_BASE + 0x200 + 4 * (n)) /* Interrupt Set-Pending Registers */
#define GICD_ICPENDR(n)          (GICD_BASE + 0x280 + 4 * (n)) /* Interrupt Clear-Pending Registers */
#define GICD_ISACTIVER(n)        (GICD_BASE + 0x300 + 4 * (n)) /* Interrupt Set-Active Registers */
#define GICD_ICACTIVER(n)        (GICD_BASE + 0x380 + 4 * (n)) /* Interrupt Clear-Active Registers */
#define GICD_IPRIORITYR(n)       (GICD_BASE + 0x400 + 4 * (n)) /* Interrupt Priority Registers */
#define GICD_ITARGETSR(n)        (GICD_BASE + 0x800 + 4 * (n)) /* Interrupt Processor Targets Registers */
#define GICD_ICFGR(n)            (GICD_BASE + 0xc00 + 4 * (n)) /* Interrupt Configuration Registers */
#define GICD_NSACR(n)            (GICD_BASE + 0xe00 + 4 * (n)) /* Non-secure Access Control Registers */
#define GICD_SGIR                (GICD_BASE + 0xf00)           /* Software Generated Interrupt Register */
#define GICD_CPENDSGIR(n)        (GICD_BASE + 0xf10 + 4 * (n)) /* SGI Clear-Pending Registers */
#define GICD_SPENDSGIR(n)        (GICD_BASE + 0xf20 + 4 * (n)) /* SGI Set-Pending Registers */

#define GICC_CTLR                (GICC_BASE + 0x0000) /* CPU Interface Control Register */
#define GICC_PMR                 (GICC_BASE + 0x0004) /* Interrupt Priority Mask Register */
#define GICC_BPR                 (GICC_BASE + 0x0008) /* Binary Point Register */
#define GICC_IAR                 (GICC_BASE + 0x000C) /* Interrupt Acknowledge Register */
#define GICC_EOIR                (GICC_BASE + 0x0010) /* End of Interrupt Register */
#define GICC_RPR                 (GICC_BASE + 0x0014) /* Running Priority Register */
#define GICC_HPPIR               (GICC_BASE + 0x0018) /* Highest Priority Pending Interrupt Register */
#define GICC_ABPR                (GICC_BASE + 0x001C) /* Aliased Binary Point Register */
#define GICC_IIDR                (GICC_BASE + 0x00FC) /* CPU Interface Identification Register */

#define GICD_CTLR_EN             (1U << 0) /* Enable forwarding of interrupts */

#define GICD_TYPER_ITLINES       /* Max number of interrupt: 32(N+1) */
#define GICD_TYPER_ITLINES_MASK  0x1fU
#define GICD_TYPER_ITLINES_SHIFT 0
#define GICD_TYPER_LSPI          /* Max number of implemented lockable SPIs */
#define GICD_TYPER_LSPI_MASK     0x1fU
#define GICD_TYPER_LSPI_SHIFT    11
#define GICD_TYPER_CPU           /* Number of implemented CPU interfaces */
#define GICD_TYPER_CPU_MASK      0x7U
#define GICD_TYPER_CPU_SHIFT     5
#define GICD_TYPER_SECEXT        (1U << 10) /* GIC implements the Security Extensions */

#define GICD_PMR_PRIOR           /* Priority mask level for the CPU interface */
#define GICD_PMR_PRIOR_MASK      0xffU
#define GICD_PMR_PRIOR_SHIFT     0

#define GICC_CTLR_EN             (1U << 0) /* Enable signaling of interrupts */

#define GICC_PMR_PRIOR           /* Priority mask level for the CPU interface */
#define GICC_PMR_PRIOR_MASK      0xffU
#define GICC_PMR_PRIOR_SHIFT     0
/* clang-format on */

#endif /* DRIVERS_INTERRUPT_GIC_GICV1_H_ */
