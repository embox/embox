/**
 * @file
 * @brief Gaisler Research GPTIMER General Purpose Timer Unit driver.
 *
 * @date 19.11.09
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include <drivers/amba_pnp.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

#include <embox/unit.h>

#define SCALER_RELOAD (SYS_CLOCK / 1000000 - 1)
#define TIMER0_RELOAD (1000000 / 1000 - 1)

#define CTRL_EN (1 << 0) /**< Enable. */
#define CTRL_RS (1 << 1) /**< Restart. */
#define CTRL_LD (1 << 2) /**< Load. */
#define CTRL_IE (1 << 3) /**< Interrupt Enable. */
#define CTRL_IP (1 << 4) /**< Interrupt Pending. */
#define CTRL_CH (1 << 5) /**< Chain. */
#define CTRL_DH (1 << 6) /**< Debug Halt. */

#define CTRL_INITIAL (CTRL_EN | CTRL_RS | CTRL_LD | CTRL_IE)

#define CFG_NTIMERS(cfg_reg) (cfg_reg & 0x7)
#define CFG_IRQ(cfg_reg)    ((cfg_reg >> 3) & 0x1f)
#define CFG_SI(cfg_reg)     ((cfg_reg >> 8) & 0x1) /**< Separate interrupts. */

/**
 * General Purpose Timer Unit registers.
 */
struct gptimer_regs {
	/**
	 * The prescaler is clocked by the system clock and decremented on each
	 * clock cycle.
	 */
	/* 0x00 */uint32_t scaler_counter;
	/**
	 * When the prescaler underflows,
	 * it is reloaded from the prescaler reload
	 * register and a timer tick is generated.
	 */
	/* 0x04 */uint32_t scaler_reload;
	/*-------+----+----+-----+---------+
	 | resrv | DF | SI | IRQ | ntimers |
	 | 31-10 | 9  | 8  | 7-3 | 2-0     |
	 +-------+----+----+-----+---------*/
	/* 0x08 */uint32_t cfg;
	/* Just a place holder. */
	/* 0x0C */uint32_t dummy;
	/**
	 * Register sets for each implemented timer.
	 */
	struct timer_entry {
		/**
		 * Timer Counter value.
		 * Decremented by 1 for each prescaler tick.
		 */
		/* 0xn0 */uint32_t counter;
		/**
		 * Timer Reload value.
		 * This value is loaded into the timer counter
		 * value register when the LD bit is written to
		 * load bit in the timers control register or when
		 * the RS bit is set in the control register and the
		 * timer underflows.
		 */
		/* 0xn4 */uint32_t reload;
		/*------+----+----+----+----+----+----+----+
		 | resv | DH | CH | IP | IE | LD | RS | EN |
		 | 31-7 | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
		 +------+----+----+----+----+----+----+----*/
		/* 0xn8 */uint32_t ctrl;
		/* Just a place holder. */
		/* 0xnC */uint32_t dummy;
	} timer[];
};

static volatile struct gptimer_regs *dev_regs;

static int dev_regs_init(unsigned int *irq_nr);

static struct time_event_device gptimer_ed;

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	// XXX clock_hander is called from arch part
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int gptimer_init(struct clock_source *cs) {
	uint32_t cfg_reg;
	unsigned int irq_nr;
	int i;

	if (NULL != dev_regs) {
		return 0;
	}

	if (0 != dev_regs_init(&irq_nr)) {
		panic("Unable to initialize gptimer dev_regs");
	}
	assert(NULL != dev_regs);

	gptimer_ed.irq_nr = irq_nr;

	cfg_reg = REG_LOAD(&dev_regs->cfg);
	for (i = 0; i < CFG_NTIMERS(cfg_reg); ++i) {
		REG_STORE(&dev_regs->timer[i].ctrl, 0x0);
	}

	REG_STORE(&dev_regs->scaler_reload, SCALER_RELOAD);
	REG_STORE(&dev_regs->scaler_counter, 0);

	return irq_attach(irq_nr, clock_handler, 0, cs, "gptimer");
}

static int gptimer_set_periodic(struct clock_source *cs);

static cycle_t gptimer_get_cycles(struct clock_source *cs) {
	return TIMER0_RELOAD - REG_LOAD(&dev_regs->timer[0].counter);
}

static struct time_event_device gptimer_ed = {
	.set_periodic = gptimer_set_periodic ,
};

static struct time_counter_device gptimer_cd = {
	.get_cycles = gptimer_get_cycles,
	.cycle_hz = 1000000,
};

static int gptimer_set_periodic(struct clock_source *cs) {
	REG_STORE(&dev_regs->timer[0].reload, TIMER0_RELOAD);
	REG_STORE(&dev_regs->timer[0].counter, 0);
	REG_STORE(&dev_regs->timer[0].ctrl, CTRL_INITIAL);
	return 0;
}

#ifdef DRIVER_AMBAPP
static int dev_regs_init(unsigned int *irq_nr) {
	amba_dev_t amba_dev;

	assert(NULL != irq_nr);
	if (-1 == capture_amba_dev(&amba_dev, AMBAPP_VENDOR_GAISLER,
			AMBAPP_DEVICE_GAISLER_GPTIMER, false, false)) {
		printk("can't capture apb dev venID=0x%X, devID=0x%X\n",
				AMBAPP_VENDOR_GAISLER, AMBAPP_DEVICE_GAISLER_GPTIMER);
		return -ENODEV;
	}
	dev_regs = (struct gptimer_regs *) amba_dev.bar[0].start;
	*irq_nr = amba_dev.dev_info.irq;
	return 0;
}
#elif OPTION_DEFINED(NUMBER,gptimer_base)
static int dev_regs_init(unsigned int *irq_nr) {
	assert(NULL != irq_nr);
	dev_regs = (volatile struct gptimer_regs *) OPTION_GET(NUMBER,gptimer_base);
	*irq_nr = OPTION_GET(NUMBER,irq_num);
	return 0;
}
#else
# error "Either DRIVER_AMBAPP or gptimer_base must be defined"
#endif /* DRIVER_AMBAPP */

CLOCK_SOURCE_DEF(gptimer, gptimer_init, NULL,
	&gptimer_ed, &gptimer_cd);
