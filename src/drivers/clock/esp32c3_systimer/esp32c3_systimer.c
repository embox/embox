#include <errno.h>
#include <stdint.h>

#include <asm/csr.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>


static struct time_event_device esp32c3_systimer_event = {};


static int esp32c3_systimer_init(struct clock_source *cs) {
	return 0;
}

CLOCK_SOURCE_DEF(esp32c3_systimer, esp32c3_systimer_init, NULL,
    &esp32c3_systimer_event, NULL);
