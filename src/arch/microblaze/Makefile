OBJS-y += interrupt_handler.o
OBJS-y += mb_irq_ctrl.o
OBJS-y += mb_timers.o
OBJS-y += entry.o
OBJS-y += boot.o
OBJS-y += sys.o

TARGETS-y += $(LDSCRIPT)

$(LDSCRIPT):$(SELFDIR)/embox.lds.S $(BUILDCONF_DIR)/config.lds.h
	$(CPP) $(CPPFLAGS) -imacros $(BUILDCONF_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -P -undef -o $@ $<

-include $(LDSCRIPT).d

include $(SCRIPTS_DIR)/common.mk
