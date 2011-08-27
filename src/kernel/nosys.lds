
PROVIDE(diag_init = nosys);
PROVIDE(diag_getc = nosys);
PROVIDE(diag_putc = nosys);

PROVIDE(irq_dispatch = nosys);

PROVIDE(softirq_init     = nosys);
PROVIDE(softirq_install  = nosys);
PROVIDE(softirq_raise    = nosys);
PROVIDE(softirq_dispatch = nosys);

PROVIDE(printk = nosys);

PROVIDE(timer_init = nosys);
PROVIDE(timer_set = nosys);
PROVIDE(timer_close = nosys);
PROVIDE(timers_off = nosys);
PROVIDE(usleep = nosys);

/* XXX remove from here. -- Eldar */
PROVIDE(uart_init = nosys);
