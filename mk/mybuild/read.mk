
ifndef __mybuild_read_mk
__mybuild_read_mk := 1

include mk/gold/engine.mk
include mk/mybuild/myfile.mk

include mk/util/wildcard.mk

ifneq ($(filter /**,$(MY_PATH)),)
$(error /** in MY_PATH)
endif

MY_FILES := $(call r-wildcard,$(MY_PATH:%=%/Mybuild))
EM_FILES := $(call r-wildcard,$(MY_PATH:%=%/Makefile))

define my_printf_escape
	$(subst $(\n),\n,
		$(subst \,\\,$1)
	)
endef
$(call def,my_printf_escape)

MKFILES_COPIES := \
	$(filter-out $(MY_FILES:$(ROOT_DIR)%Mybuild=$(EM_DIR)%Makefile), \
		$(EM_FILES:$(ROOT_DIR)%=$(EM_DIR)%))

MKFILES_CONVERTED := $(MY_FILES:$(ROOT_DIR)%Mybuild=$(EM_DIR)%Makefile)

MKFILES := $(MKFILES_CONVERTED) $(MKFILES_COPIES)

$(MKFILES_COPIES) : \
		$(EM_DIR)% : $(ROOT_DIR)%
	@mkdir -p $(@D); cp $< $@

$(MKFILES_CONVERTED) : mk/mybuild/read.mk mk/mybuild/myfile.mk
$(MKFILES_CONVERTED) : \
		$(EM_DIR)%Makefile : $(ROOT_DIR)%Mybuild
	@echo '$< -> $@'
	@mkdir -p $(@D); $(PRINTF) '%b' '$(call my_printf_escape,$(call gold_parse_file,myfile,$<))' > $@

endif # __mybuild_read_mk
