/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.05.25
 */
#include <util/log.h>

#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>

#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <util/field.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#define TMR_IDX       0

#define BASE_ADDR     ((uintptr_t)OPTION_GET(NUMBER,base_addr))
#define IRQ_NUM        OPTION_GET(NUMBER,irq_num)

#define VG1T_VERSION   OPTION_GET(NUMBER, vg1t_version)



#if VG1T_VERSION == 1

#define TMR_CTRL         (BASE_ADDR + 0x00) /* Timer Control Register */
#define TMR_COUNT        (BASE_ADDR + 0x04) /* Current Value Timer Register */
#define TMR_CLKDIV       (BASE_ADDR + 0x08)
#define TMR_PERIOD       (BASE_ADDR + 0x0C)
#define TMR_IM           (BASE_ADDR + 0x10) /* Interrupt Mask Register */
#define TMR_RIS          (BASE_ADDR + 0x14) /* Raw Interrupt Status Register */
#define TMR_MIS          (BASE_ADDR + 0x18) /* Masked Interrupt Status Register */
#define TMR_IC           (BASE_ADDR + 0x1C) /* Clear Interrupt Status Register */
#define TMR_CAPCOM0_CTRL (BASE_ADDR + 0x100) /* Capture/Compare Control Register */
#define TMR_CAPCOM0_VAL0 (BASE_ADDR + 0x104) /* Capture/Compare Value Register */
#define TMR_CAPCOM0_VAL1 (BASE_ADDR + 0x108) /* Capture/Compare Value Register */

#else
/* clang-format off */
#define TMR_CTRL         (BASE_ADDR + 0x00) /* Timer Control Register */
#define TMR_COUNT        (BASE_ADDR + 0x04) /* Current Value Timer Register */
#define TMR_IM           (BASE_ADDR + 0x08) /* Interrupt Mask Register */
#define TMR_RIS          (BASE_ADDR + 0x0c) /* Raw Interrupt Status Register */
#define TMR_MIS          (BASE_ADDR + 0x10) /* Masked Interrupt Status Register */
#define TMR_IC           (BASE_ADDR + 0x14) /* Clear Interrupt Status Register */
#define TMR_CAPCOM0_CTRL (BASE_ADDR + 0x18) /* Capture/Compare Control Register */
#define TMR_CAPCOM0_VAL  (BASE_ADDR + 0x1c) /* Capture/Compare Value Register */
#define TMR_DMA_IM       (BASE_ADDR + 0x38) /* DMA Request Mask Register */
#define TMR_ADC_IM       (BASE_ADDR + 0x3c) /* ADC Request Mask Register */

#endif

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

#define TMR_PERIOD_VALUE ((SYS_CLOCK / JIFFIES_PERIOD) - 1)

extern int sys_ctrl_enable_tmr(int num) ;
extern int sys_ctrl_enable_dev(const char *name);

static int niiet_tmr_set_periodic(struct clock_source *cs) {

	REG32_STORE(TMR_CTRL, 0);
	REG32_STORE(TMR_IC, 0x1FF);
	REG32_STORE(TMR_PERIOD, TMR_PERIOD_VALUE);
	REG32_STORE(TMR_CTRL, FIELD(TMR_CTRL_MODE, TMR_CTRL_MODE_UP));

	REG32_STORE(TMR_IM, TMR_IM_TMR);

	return 0;
}

static cycle_t niiet_tmr_get_cycles(struct clock_source *cs) {
	return (cycle_t)REG32_LOAD(TMR_COUNT);
}

static struct time_event_device niiet_tmr_event = {
    .set_periodic = niiet_tmr_set_periodic,
    .irq_nr = IRQ_NUM,
};

static struct time_counter_device niiet_tmr_counter = {
    .get_cycles = niiet_tmr_get_cycles,
    .cycle_hz = TMR_PERIOD,
};

static irq_return_t niiet_tmr_irq_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);

	REG32_STORE(TMR_IC, TMR_IC_TMR);

	return IRQ_HANDLED;
}

static int niiet_tmr_init(struct clock_source *cs) {
	extern void niiet_tmr_set_rcu(int);

	niiet_tmr_set_rcu(TMR_IDX);

	sys_ctrl_enable_tmr(TMR_IDX);

	return irq_attach(IRQ_NUM, niiet_tmr_irq_handler, 0, cs, "tmr32");
}

CLOCK_SOURCE_DEF(tmr32, niiet_tmr_init, NULL, &niiet_tmr_event, &niiet_tmr_counter);

PERIPH_MEMORY_DEFINE(tmr32, BASE_ADDR, 0x40);
