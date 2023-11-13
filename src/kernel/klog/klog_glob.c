/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <kernel/printk.h>
#include <util/log.h>

static void klog_handler(struct logger *logger, log_prio_t prio,
    const char *fmt, va_list args) {
	printk("%s", log_prio_prefix(prio));
	vprintk(fmt, args);
	printk("\n");
}

void klog_init(void) {
	log_set_handler(klog_handler);
}
