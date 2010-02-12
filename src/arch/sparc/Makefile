OBJS-y += sparc_traps.o
OBJS-$(MONITOR_SYSTEM_IRQ) += apb_irq_ctrl.o
OBJS-$(MONITOR_SYSTEM_TIMERS) += apb_timers.o
OBJS-y += boot.o
OBJS-y += cpu_context.o
OBJS-y += entry.o
OBJS-y += trap_setup.o
OBJS-y += windows.o
OBJS-y += bubble_sort.o
OBJS-y += sys.o
OBJS-y += soft_trap_test.o

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $(SELFDIR)/embox.lds.S $(AUTOCONF_DIR)/config.lds.h
	$(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d

include $(SCRIPTS_DIR)/common.mk
