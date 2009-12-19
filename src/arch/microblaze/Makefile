OBJS-y += interrupt_handler.o
OBJS-y += mb_irq_ctrl.o
OBJS-y += mb_timers.o
OBJS-y += entry.o
OBJS-y += boot.o
OBJS-y += sys.o

-include $(LDSCRIPT).d

$(LDSCRIPT):$(SELFDIR)/embox.lds.S $(BUILDCONF_DIR)/config.lds.h
	$(CC) $(CCINCLUDES) -imacros $(BUILDCONF_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -E -Wp, -P -undef -o $@ $<

include $(SCRIPTS_DIR)/common.mk
