OBJS-y += interrupt_handler.o
OBJS-y += mb_irq_ctrl.o
OBJS-y += mb_timers.o
OBJS-y += entry.o
OBJS-y += boot.o
OBJS-y += sys.o

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $(SELFDIR)/embox.lds.S $(BUILDCONF_DIR)/config.lds.h
	$(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(BUILDCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d

include $(SCRIPTS_DIR)/common.mk
