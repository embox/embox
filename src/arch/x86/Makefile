
# Auto-generated package declaration inherited from parent directory.
# TODO Suggested renaming package to arch.x86
$_PACKAGE := embox.hal


SUBDIRS := $(filter-out include,$(SUBDIRS))

$(IMAGE): $(LDSCRIPT)

$(LDSCRIPT): $($_SELFDIR)/embox.lds.S $(AUTOCONF_DIR)/config.lds.h
	$(CPP) -P -undef $(CPPFLAGS) \
	        -imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $(LDSCRIPT).d
