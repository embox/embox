/**
 * @file
 * @brief
 *
 * @author zhugengyu
 * @date 05.09.2026
 */

#ifndef DRIVERS_INTERRUPT_GIC_GIC_LPI_H_
#define DRIVERS_INTERRUPT_GIC_GIC_LPI_H_

#include <sys/types.h>

/* First LPI INTID in the GICv3 INTID space. LPI INTIDs are assigned
 * by the ITS and never appear in the GICD/GICR enable registers. */
#define GIC_LPI_INTID_BASE 8192

/*
 * Kernel IRQ numbers reserved for LPI delivery. The kernel IRQ space
 * is a flat array indexed by INTID, far too small to hold raw LPI
 * INTIDs, so the ITS driver maps the LPIs it allocates one-to-one
 * onto this window. Board templates must keep their SPI assignments
 * below GIC_LPI_IRQ_BASE.
 */
#define GIC_LPI_IRQ_BASE 208

/**
 * Translate a raw LPI INTID to the kernel IRQ number it is bound to.
 *
 * @param intid INTID as read from ICC_IAR1_EL1 (>= GIC_LPI_INTID_BASE)
 *
 * @return Kernel IRQ number, or -1 when the INTID has no mapping
 *     (treated as spurious by the caller).
 */
extern int gic_lpi_intid_to_irq(unsigned int intid);

/**
 * Translate a kernel IRQ number from the LPI window back to the raw
 * LPI INTID. Used before writing ICC_EOIR1_EL1, which expects the
 * INTID that was acknowledged.
 */
extern unsigned int gic_lpi_irq_to_intid(unsigned int irq);

/**
 * Enable or disable delivery of an LPI-bound kernel IRQ number.
 */
extern void gic_lpi_set_state(unsigned int irq, int enable);

#endif /* DRIVERS_INTERRUPT_GIC_GIC_LPI_H_ */
