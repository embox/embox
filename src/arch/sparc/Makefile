OBJS-y += sparc_traps.o
OBJS-y += apb_irq_ctrl.o
OBJS-y += apb_timers.o
OBJS-y += boot.o
OBJS-y += cpu_context.o
OBJS-y += entry.o
OBJS-y += trap_setup.o
OBJS-y += windows.o
OBJS-y += bubble_sort.o
OBJS-y += sys.o
OBJS-y += soft_trap_test.o

TARGETS-y += $(LDSCRIPT)

$(LDSCRIPT):$(SELFDIR)/embox.lds.S $(BUILDCONF_DIR)/config.lds.h
	$(CPP) $(CPPFLAGS) -imacros $(BUILDCONF_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -P -undef -o $@ $<

-include $(LDSCRIPT).d

include $(SCRIPTS_DIR)/common.mk
