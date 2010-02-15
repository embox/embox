$_MODS_ESSENTIAL += locore

$_SUBDIRS := $(filter-out include,$($_SUBDIRS))

$_SRCS-locore += interrupt_handler.c sys.c entry.S boot.S
$_SRCS-interrupt += mb_irq_ctrl.c
$_SRCS-clock += mb_timers.c
$_SRCS-arch += arch.c
$_SRCS-diag += diag_uart.c

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $(SELFDIR)/embox.lds.S $(AUTOCONF_DIR)/config.lds.h
	$(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d

include $(SCRIPTS_DIR)/common.mk
