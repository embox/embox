#
# Loads my- and config-files, links them together, and infers the build model.
#
#   Date: Feb 9, 2012
# Author: Eldar Abusalimov
# Author: Anton Kozlov
#

MYFILES_PATH := mk/ src/ platform/ third-party/
MYFILES := \
	$(shell find $(MYFILES_PATH) -depth \
		\( -name Mybuild -o -name \*.my \) -print)

CONFIGFILES_PATH := $(CONF_DIR)
CONFIGFILES := $(wildcard $(CONFIGFILES_PATH)/*.config)

ifeq ($(strip $(CONFIGFILES)),)
$(error No config files were found in '$(CONFIGFILES_PATH)')
endif

ifneq ($(word 2,$(CONFIGFILES)),)
$(error Handling multiplie config files is not implemented: '$(CONFIGFILES)')
endif

#
# Directories where to put generated scripts.
#
ifndef CACHE_DIR
$(error CACHE_DIR is not defined, \
	do not include this script directly, use mk/load.mk instead)
endif

export MYBUILD_CACHE_DIR       := $(CACHE_DIR)/mybuild
export MYBUILD_FILES_CACHE_DIR := $(MYBUILD_CACHE_DIR)/files

#
# Generated artifacts.
#


annotations_core_mk := $(MYBUILD_FILES_CACHE_DIR)/annotations_core.mk
$(annotations_core_mk) : MAKEFILES := $(mk_annotations_core) $(mk_annotations_handlers_mk)
$(annotations_core_mk) :
	@echo ' ANNOTATION CORE'
	@$(MAKE) -f mk/script/mk-persist.mk \
		PERSIST_OBJECTS='$$(call new,AnnotationsCore,$(mk_annotations_handlers))' \
		PERSIST_REALLOC='hnd' \
		PERSIST_VARIABLE='annotationsCore' \
		ALLOC_SCOPE='hnd' > $@
load_mybuild_files += $(annotations_core_mk)

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
$(myfiles_mk) $(configfiles_mk) : mk/load-mybuild.inc.mk
$(myfiles_mk) $(configfiles_mk) : mk/script/mk-persist.mk
$(myfiles_mk) $(configfiles_mk) : | $$(@D)/.

$(myfiles_mk) $(configfiles_mk) : $(MYBUILD_FILES_CACHE_DIR)/%.mk : %
	@echo ' $(recipe_tag) $<'
	@SCOPE=`echo '$<' | md5sum | cut -c -8`; \
	$(MAKE) -f mk/script/mk-persist.mk ALLOC_SCOPE="@$$SCOPE" > $@ && \
	echo '__resource-$@ := '".obj1@$$SCOPE" >> $@

#
# Linking files together.
#

# My-files.
export myfiles_model_mk := $(MYBUILD_CACHE_DIR)/myfiles-model.mk
myfiles_mk_cached_list_mk := $(MYBUILD_CACHE_DIR)/myfiles-list.mk

$(myfiles_model_mk) : MAKEFILES := $(mk_mybuild_myfile) $(myfiles_mk) \
	$(annotations_core_mk) $(mk_annotations_handlers_mk)
$(myfiles_model_mk) :
	@echo ' MYLINK: $(words $(myfiles_mk)) files $(__myfiles_model_stats)'
	@$(MAKE) -f mk/script/mk-persist.mk \
		PERSIST_OBJECTS='$$(call myfile_create_resource_set, \
				$$(foreach f,$(myfiles_mk),$$(__resource-$$f)))' \
		PERSIST_REALLOC='my' \
		PERSIST_VARIABLE='__myfile_resource_set' \
		ALLOC_SCOPE='z' > $@
	@$(PRINTF) 'myfiles_mk_cached := %b' '$(myfiles_mk:%=\\\n\t%)' \
		> $(myfiles_mk_cached_list_mk)
load_mybuild_files += $(myfiles_model_mk)

# Config-files are linked agains linked model of my-files.
export configfiles_model_mk := $(MYBUILD_CACHE_DIR)/configfiles-model.mk

$(configfiles_model_mk) : MAKEFILES := $(mk_mybuild) $(configfiles_mk) $(myfiles_model_mk)
$(configfiles_model_mk) :
	@echo ' CONFIGLINK'
	@$(MAKE) -f mk/script/mk-persist.mk \
		PERSIST_OBJECTS='$$(call config_create_resource_set, \
				$$(foreach f,$(configfiles_mk),$$(__resource-$$f)), \
				$$(__myfile_resource_set))' \
		PERSIST_REALLOC='cfg' \
		PERSIST_VARIABLE='__config_resource_set' \
		ALLOC_SCOPE='y' > $@
load_mybuild_files += $(configfiles_model_mk)

# Build model is inferred from both configuration and myfiles models.
export build_model_mk := $(MYBUILD_CACHE_DIR)/build-model.mk

$(build_model_mk) : MAKEFILES := $(mk_mybuild) $(configfiles_model_mk) $(myfiles_model_mk) \
	$(annotations_core_mk) $(mk_annotations_handlers_mk)
$(build_model_mk) :
	@echo ' BUILDMODEL'
	@$(MAKE) -f mk/script/mk-persist.mk \
		PERSIST_OBJECTS='$$(call mybuild_create_build, \
				$$(__config_resource_set))' \
		PERSIST_REALLOC='bld' \
		PERSIST_VARIABLE='build_model' \
		ALLOC_SCOPE='x' > $@
load_mybuild_files += $(build_model_mk)

export load_mybuild_files := $(load_mybuild_files)

$(load_mybuild_files) : export MAKEFILES ?=

$(load_mybuild_files) : $$(MAKEFILES)
$(load_mybuild_files) : mk/load-mybuild.inc.mk
$(load_mybuild_files) : mk/script/mk-persist.mk
$(load_mybuild_files) : | $$(@D)/.

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

