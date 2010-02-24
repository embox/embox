
$_SUBDIRS := $(filter-out include,$($_SUBDIRS))

$_MODS_CORE += locore

$_SRCS-locore += interrupt_handler.c sys.c entry.S boot.S
$_SRCS-interrupt += mb_irq_ctrl.c
$_SRCS-clock += mb_timers.c
$_SRCS-arch += arch.c

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $($_SELFDIR)/embox.lds.S $(AUTOCONF_DIR)/config.lds.h
	$(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d
