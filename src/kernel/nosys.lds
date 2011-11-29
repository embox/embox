
PROVIDE(irq_dispatch = nosys);

PROVIDE(printk = nosys);

PROVIDE(timer_init = nosys);
PROVIDE(timer_set = nosys);
PROVIDE(timer_close = nosys);
PROVIDE(timers_off = nosys);
PROVIDE(usleep = nosys);

/* XXX remove from here. -- Eldar
PROVIDE(uart_init = nosys);
*/
