OBJS-y += interrupt_handler.o
OBJS-$(MONITOR_SYSTEM_IRQ) += mb_irq_ctrl.o
OBJS-$(MONITOR_SYSTEM_TIMERS) += mb_timers.o
OBJS-y += entry.o
OBJS-y += boot.o
OBJS-y += sys.o

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $(SELFDIR)/embox.lds.S $(AUTOCONF_DIR)/config.lds.h
	$(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d

include $(SCRIPTS_DIR)/common.mk
