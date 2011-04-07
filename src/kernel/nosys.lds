
PROVIDE(diag_init = nosys);
PROVIDE(diag_getc = nosys);
PROVIDE(diag_putc = nosys);

PROVIDE(irq_init     = nosys);
PROVIDE(irq_attach   = nosys);
PROVIDE(irq_detach   = nosys);
PROVIDE(irq_dispatch = nosys);

PROVIDE(softirq_init     = nosys);
PROVIDE(softirq_install  = nosys);
PROVIDE(softirq_raise    = nosys);
PROVIDE(softirq_dispatch = nosys);

PROVIDE(sched_policy_current = nosys);
PROVIDE(__sched_dispatch = nosys);

PROVIDE(printk = nosys);

PROVIDE(timer_init = nosys);
PROVIDE(set_timer = nosys);
PROVIDE(close_timer = nosys);
PROVIDE(timers_off = nosys);
PROVIDE(irq_func_tmr_1mS = nosys);
PROVIDE(usleep = nosys);

/* XXX remove from here. -- Eldar */
PROVIDE(uart_init = nosys);
