/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.05.25
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <util/field.h>
#include <util/log.h>

#define TMR32_BASE 0x30000000UL
#define TMR32_IRQ  6

/* clang-format off */
#define TMR_CTRL         (TMR32_BASE + 0x00) /* Timer Control Register */
#define TMR_COUNT        (TMR32_BASE + 0x04) /* Current Value Timer Register */
#define TMR_IM           (TMR32_BASE + 0x08) /* Interrupt Mask Register */
#define TMR_RIS          (TMR32_BASE + 0x0c) /* Raw Interrupt Status Register */
#define TMR_MIS          (TMR32_BASE + 0x10) /* Masked Interrupt Status Register */
#define TMR_IC           (TMR32_BASE + 0x14) /* Clear Interrupt Status Register */
#define TMR_CAPCOM0_CTRL (TMR32_BASE + 0x18) /* Capture/Compare Control Register */
#define TMR_CAPCOM0_VAL  (TMR32_BASE + 0x1c) /* Capture/Compare Value Register */
#define TMR_DMA_IM       (TMR32_BASE + 0x38) /* DMA Request Mask Register */
#define TMR_ADC_IM       (TMR32_BASE + 0x3c) /* ADC Request Mask Register */
/* clang-format on */

#define TMR_CAPCOM_CTRL(n) (TMR_CAPCOM0_CTRL + 8 * n) /* (n: 0..3) */
#define TMR_CAPCOM_VAL(n)  (TMR_CAPCOM0_VAL + 8 * n)  /* (n: 0..3) */

#define TMR_CTRL_CLKSEL (1 << 8) /* Clock Source Select */
#define TMR_CTRL_CLR    (1 << 2) /* Timer Clear */

#define TMR_CTRL_DIV       /* Divider */
#define TMR_CTRL_DIV_MASK  0x3
#define TMR_CTRL_DIV_SHIFT 6

#define TMR_CTRL_MODE       /* Count Mode */
#define TMR_CTRL_MODE_MASK  0x3
#define TMR_CTRL_MODE_SHIFT 4
#define TMR_CTRL_MODE_STOP  0
#define TMR_CTRL_MODE_UP    1

#define TMR_IM_CAP3 (1 << 4) /* Capcom3 Interrupt Enable */
#define TMR_IM_CAP2 (1 << 3) /* Capcom2 Interrupt Enable */
#define TMR_IM_CAP1 (1 << 2) /* Capcom1 Interrupt Enable */
#define TMR_IM_CAP0 (1 << 1) /* Capcom0 Interrupt Enable */
#define TMR_IM_TMR  (1 << 0) /* Timer Interrupt Enable */

#define TMR_RIS_CAP3 (1 << 4) /* Capcom3 Raw Interrupt Status */
#define TMR_RIS_CAP2 (1 << 3) /* Capcom2 Raw Interrupt Status */
#define TMR_RIS_CAP1 (1 << 2) /* Capcom1 Raw Interrupt Status */
#define TMR_RIS_CAP0 (1 << 1) /* Capcom0 Raw Interrupt Status */
#define TMR_RIS_TMR  (1 << 0) /* Timer Raw Interrupt Status */

#define TMR_MIS_CAP3 (1 << 4) /* Capcom3 Masked Interrupt Status */
#define TMR_MIS_CAP2 (1 << 3) /* Capcom2 Masked Interrupt Status */
#define TMR_MIS_CAP1 (1 << 2) /* Capcom1 Masked Interrupt Status */
#define TMR_MIS_CAP0 (1 << 1) /* Capcom0 Masked Interrupt Status */
#define TMR_MIS_TMR  (1 << 0) /* Timer Masked Interrupt Status */

#define TMR_IC_CAP3 (1 << 4) /* Capcom3 Interrupt Clear */
#define TMR_IC_CAP2 (1 << 3) /* Capcom2 Interrupt Clear */
#define TMR_IC_CAP1 (1 << 2) /* Capcom1 Interrupt Clear */
#define TMR_IC_CAP0 (1 << 1) /* Capcom0 Interrupt Clear */
#define TMR_IC_TMR  (1 << 0) /* Timer Interrupt Clear */

#define TMR_CAPCOM_CTRL_SCCI (1 << 10) /* Synchronized Capture/Compare Input */
#define TMR_CAPCOM_CTRL_CAP  (1 << 8)  /* Capture Mode Enable */
#define TMR_CAPCOM_CTRL_CCI  (1 << 3)  /* Capture/Compare Input */
#define TMR_CAPCOM_CTRL_OUT  (1 << 2)  /* Output */
#define TMR_CAPCOM_CTRL_OVF  (1 << 1)  /* Capture Overflow */

#define TMR_CAPCOM_CTRL_CAPMODE       /* Capture Mode */
#define TMR_CAPCOM_CTRL_CAPMODE_MASK  0x3
#define TMR_CAPCOM_CTRL_CAPMODE_SHIFT 14

#define TMR_CAPCOM_CTRL_CCISEL       /* Capture/Compare Input Select */
#define TMR_CAPCOM_CTRL_CCISEL_MASK  0x3
#define TMR_CAPCOM_CTRL_CCISEL_SHIFT 12

#define TMR_CAPCOM_CTRL_OUTMODE       /* Output Mode */
#define TMR_CAPCOM_CTRL_OUTMODE_MASK  0x7
#define TMR_CAPCOM_CTRL_OUTMODE_SHIFT 5

#define TMR_DMA_IM_CAP3 (1 << 4) /* Capcom3 DMA Request Enable */
#define TMR_DMA_IM_CAP2 (1 << 3) /* Capcom2 DMA Request Enable */
#define TMR_DMA_IM_CAP1 (1 << 2) /* Capcom1 DMA Request Enable */
#define TMR_DMA_IM_CAP0 (1 << 1) /* Capcom0 DMA Request Enable */
#define TMR_DMA_IM_TMR  (1 << 0) /* Timer DMA Request Enable */

#define TMR_PERIOD ((SYS_CLOCK / JIFFIES_PERIOD) - 1)

static int tmr32_set_periodic(struct clock_source *cs) {
	REG32_STORE(TMR_CAPCOM0_VAL, TMR_PERIOD);
	REG32_STORE(TMR_CTRL, FIELD(TMR_CTRL_MODE, TMR_CTRL_MODE_UP));
	REG32_STORE(TMR_IM, TMR_IM_CAP0);

	return 0;
}

static cycle_t tmr32_read(struct clock_source *cs) {
	return (cycle_t)REG32_LOAD(TMR_COUNT);
}

static struct time_event_device tmr32_event = {
    .set_periodic = tmr32_set_periodic,
    .irq_nr = TMR32_IRQ,
};

static struct time_counter_device tmr32_counter = {
    .read = tmr32_read,
    .cycle_hz = TMR_PERIOD,
};

static irq_return_t tmr32_irq_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);

	REG32_STORE(TMR_IC, TMR_IC_CAP0);

	return IRQ_HANDLED;
}

static int tmr32_init(struct clock_source *cs) {
	extern void niiet_tmr32_set_rcu(void);

	niiet_tmr32_set_rcu();

	return irq_attach(TMR32_IRQ, tmr32_irq_handler, 0, cs, "tmr32");
}

CLOCK_SOURCE_DEF(tmr32, tmr32_init, NULL, &tmr32_event, &tmr32_counter);

PERIPH_MEMORY_DEFINE(tmr32, PTIMER_BASE_ADDR, 0x40);
