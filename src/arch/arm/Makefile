
SUBDIRS := $(filter-out include,$(SUBDIRS))

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $($_SELFDIR)/embox.lds.S $(AUTOCONF_DIR)/config.lds.h
	$(CPP) -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d
