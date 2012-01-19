
ifndef __mybuild_read_mk
__mybuild_read_mk := 1

include mk/gold/engine.mk
include mk/mybuild/myfile.mk
include mk/util/serialize.mk

include mk/util/wildcard.mk

ifneq ($(filter /**,$(MY_PATH)),)
$(error /** in MY_PATH)
endif

MY_FILES := $(call r-wildcard,$(MY_PATH:%=%/Mybuild) $(MY_PATH:%=%/*.my))
EM_FILES := $(call r-wildcard,$(MY_PATH:%=%/Makefile))

# 1. File names.
# 2. List.
my_filter = \
	$(call my_filter_only,$(SRC_DIR)/arch,$1 $(ARCH)/, \
		$(call my_filter_only,$(PLATFORM_DIR),$1 $(PLATFORM)/,$2))

# 1. Parent directory.
# 2. Name.
# 3. List
my_filter_only = \
	$(filter-out $1/%,$3) $(filter $(2:%=$1/%%),$3)

MY_FILES := $(call my_filter,Mybuild .my,$(MY_FILES))
EM_FILES := $(call my_filter,Makefile,$(EM_FILES))

MKFILES_COPIES := \
	$(filter-out $(MY_FILES:$(ROOT_DIR)%Mybuild=$(EM_DIR)%Makefile), \
		$(EM_FILES:$(ROOT_DIR)%=$(EM_DIR)%))

MKFILES_CONVERTED := \
	$(call filter-patsubst,$(ROOT_DIR)%Mybuild,$(EM_DIR)%Makefile,$(MY_FILES)) \
	$(call filter-patsubst,$(ROOT_DIR)%.my,$(EM_DIR)%.my.mk,$(MY_FILES))

MKFILES := $(MKFILES_CONVERTED) $(MKFILES_COPIES)

-include $(MKFILES)

MK_LINK := $(EM_DIR)/linked.mk

-include $(MK_LINK)

$(MKFILES_COPIES) : \
		$(EM_DIR)% : $(ROOT_DIR)%
	@mkdir -p $(@D); cp $< $@

$(MKFILES_CONVERTED) : mk/mybuild/read.mk mk/mybuild/myfile.mk

$(filter %Makefile,$(MKFILES_CONVERTED)) : \
		$(EM_DIR)%Makefile : $(ROOT_DIR)%Mybuild
	@echo '$< -> $@'
	$(eval $@ := $$(call create_from_model,$$(call gold_parse,myfile,$$<)))
	$(info $($@))
	@mkdir -p $(@D); $(PRINTF) '%b' '$(call escape_printf, $(call objects_to_mk,$($@)))' > $@

$(filter %.my.mk,$(MKFILES_CONVERTED)) : \
		$(EM_DIR)%.my.mk : $(ROOT_DIR)%.my
	@echo '$< -> $@'
	$(eval $@ := $$(call create_from_model,$$(call gold_parse,myfile,$$<)))
	$(info $($@))
	@mkdir -p $(@D); $(PRINTF) '%b' '$(call escape_printf, $(call objects_to_mk,$($@)))' > $@


$(MK_LINK) : $(MKFILES_CONVERTED)
	@echo '... -> $@'
	@$(PRINTF) '%b' '$(call escape_printf, \
		$(call raw_objects_to_mk,$(call resolve_links_from_files,$^)))' > $@

endif # __mybuild_read_mk
