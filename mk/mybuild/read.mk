
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

# 1. File name.
# 2. List.
my_filter = \
	$(call my_filter_only,$(SRC_DIR)/arch,$1 $(ARCH)/, \
		$(call my_filter_only,$(PLATFORM_DIR),$1 $(PLATFORM)/,$2))

# 1. Parent directory.
# 2. Name.
# 3. List
my_filter_only = \
	$(filter-out $1/%,$3) $(filter $(2:%=$1/%%),$3)

MY_FILES := $(call my_filter,Mybuild,$(MY_FILES))
EM_FILES := $(call my_filter,Makefile,$(EM_FILES))

define my_printf_escape
	$(or \
		$(subst $(\n),\n,
			$(subst \,\\,$1)
		),
		$(error Empty input)
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
	@mkdir -p $(@D); $(PRINTF) '%b' '$(call my_printf_escape,$(call gold_parse,myfile,$<))' > $@

endif # __mybuild_read_mk
