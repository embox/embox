#
# Second stage: loads my- and config-files and links them together.
#
#   Date: Feb 9, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

MYFILES_PATH := mk/ src/ platform/ third-party/
MYFILES := \
	$(shell find $(MYFILES_PATH) -depth \
		\( -name Mybuild -o -name \*.my \) -print)

ifeq (1,1) #######

CONFIG_PATH := conf
CONFIG_GENERATED_PATH := build/base/codegen

HOSTCPP = gcc -E
HOSTCPPFLAGS := -I$(CONFIG_PATH)

CONFIG_GENERATED := $(CONFIG_GENERATED_PATH)/genConf.config

$(CONFIG_GENERATED) :
	mkdir -p $(@D)
	$(HOSTCPP) -P -undef -nostdinc $(HOSTCPPFLAGS) \
		-MMD -MT $@ -MF $@.d -D__MODS_CONF__ mk/confmacro2.S | \
		awk -f mk/confmacro2.awk > $@

-include $(CONFIG_GENERATED:%=%.d)

#CONFIGFILES := \
#	$(shell find $(CONFIG_PATH) -depth \
#		-name \*.config -print) \
#	$(CONFIG_GENERATED)
#CONFIGFILES := $(firstword $(CONFIGFILES))

CONFIGFILES := $(CONFIG_GENERATED)

endif #######

#
# Directory where to put generated scripts.
#
export MYBUILD_CACHE_DIR       := $(CACHE_DIR)/mybuild
export MYBUILD_FILES_CACHE_DIR := $(MYBUILD_CACHE_DIR)/files

#
# Generated artifacts.
#

.DELETE_ON_ERROR:
.SECONDEXPANSION:

# My-files.
myfiles_mk := \
	$(patsubst $(abspath ./%),$(MYBUILD_FILES_CACHE_DIR)/%.mk, \
		$(abspath $(MYFILES)))
$(myfiles_mk) : recipe_tag      := MYFILE
$(myfiles_mk) : MAKEFILES       := $(mk_mybuild_myfile)
$(myfiles_mk) : PERSIST_OBJECTS  = $$(call new,MyFileResource,$<)

# Config-files.
configfiles_mk := \
	$(patsubst $(abspath ./%),$(MYBUILD_FILES_CACHE_DIR)/%.mk, \
		$(abspath $(CONFIGFILES)))
$(configfiles_mk) : recipe_tag      := CONFIGFILE
$(configfiles_mk) : MAKEFILES       := $(mk_mybuild_configfile)
$(configfiles_mk) : PERSIST_OBJECTS  = $$(call new,ConfigFileResource,$<)

# Defaults, must be overridden with target-specific variables.
$(myfiles_mk) $(configfiles_mk) : export PERSIST_OBJECTS ?=
$(myfiles_mk) $(configfiles_mk) : export MAKEFILES ?=

$(myfiles_mk) $(configfiles_mk) : $$(MAKEFILES)
$(myfiles_mk) $(configfiles_mk) : mk/load2.mk
$(myfiles_mk) $(configfiles_mk) : mk/script/mk-persist.mk

$(myfiles_mk) $(configfiles_mk) : $(MYBUILD_FILES_CACHE_DIR)/%.mk : %
	@echo ' $(recipe_tag) $<'
	@SCOPE=`echo '$<' | sum | cut -f 1 -d ' '`; \
	mkdir -p $(@D) && \
	$(MAKE) -f mk/script/mk-persist.mk ALLOC_SCOPE="p$$SCOPE" > $@ && \
	echo '$$(lastword $$(MAKEFILE_LIST)) := '".obj1p$$SCOPE" >> $@

#
# Linking files together.
#

# My-files.
myfiles_model_mk := $(MYBUILD_CACHE_DIR)/myfiles-model.mk
myfiles_mk_cached_list_mk := $(MYBUILD_CACHE_DIR)/myfiles-list.mk

$(myfiles_model_mk) : MAKEFILES := $(mk_mybuild) $(myfiles_mk)
$(myfiles_model_mk) : $(myfiles_mk)
	@echo ' MYLINK: $(words $(myfiles_mk)) files $(__myfiles_model_stats)'
	@mkdir -p $(@D) && \
		$(MAKE) -f mk/script/mk-persist.mk \
		PERSIST_OBJECTS='$$(call myfile_create_resource_set_from_files,$(myfiles_mk))' \
		PERSIST_REALLOC='my' \
		ALLOC_SCOPE='z' > $@
	@echo '__myfile_resource_set := .my1z' >> $@
	@printf 'myfiles_mk_cached := %b' '$(myfiles_mk:%=\\\n\t%)' \
		> $(myfiles_mk_cached_list_mk)

# Config-files are linked agains linked model of my-files.
configfiles_model_mk := $(MYBUILD_CACHE_DIR)/configfiles-model.mk

$(configfiles_model_mk) : MAKEFILES := $(mk_mybuild) $(configfiles_mk) $(myfiles_model_mk)
$(configfiles_model_mk) : $(configfiles_mk)
	@echo ' CONFIGLINK'
	@mkdir -p $(@D) && \
		$(MAKE) -f mk/script/mk-persist.mk \
		PERSIST_OBJECTS='$$(call config_create_resource_set_from_files,$(configfiles_mk))' \
		PERSIST_REALLOC='cfg' \
		ALLOC_SCOPE='y' > $@
	@echo '__config_resource_set := .cfg1y' >> $@

$(myfiles_model_mk) $(configfiles_model_mk) : export MAKEFILES ?=

$(myfiles_model_mk) $(configfiles_model_mk) : $$(MAKEFILES)
$(myfiles_model_mk) $(configfiles_model_mk) : mk/load2.mk
$(myfiles_model_mk) $(configfiles_model_mk) : mk/script/mk-persist.mk

$(MAKECMDGOALS) : $(myfiles_model_mk) $(configfiles_model_mk)
	@$(MAKE) -f mk/main.mk MAKEFILES='$(all_mk_files) $^' $@

#
# Added/removed myfiles detection.
#

-include $(myfiles_mk_cached_list_mk)
myfiles_mk_cached ?=

export myfiles_mk_added := \
	$(filter-out $(myfiles_mk_cached),$(myfiles_mk))
export myfiles_mk_removed := \
	$(filter-out $(myfiles_mk),$(myfiles_mk_cached))

ifneq ($(or $(myfiles_mk_added),$(myfiles_mk_removed)),)
.PHONY : $(myfiles_model_mk)
ifneq ($(filter-out recursive,$(flavor myfiles_mk_cached)),)
$(myfiles_model_mk) : __myfiles_model_stats := ($(if \
		$(myfiles_mk_added),$(words $(myfiles_mk_added)) added)$(and \
		$(myfiles_mk_added),$(myfiles_mk_removed),$(if ,,, ))$(if \
		$(myfiles_mk_removed),$(words $(myfiles_mk_removed)) removed))
endif
endif

$(myfiles_model_mk) : __myfiles_model_stats ?=

