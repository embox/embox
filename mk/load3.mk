#
# Third stage: loads conf-files.
#
#   Date: Feb 9, 2012
# Author: Eldar Abusalimov
#

export CONFIGFILES_CACHE_DIR := $(MYBUILD_CACHE_DIR)/config

CONFIG_PATH := conf/

CONFIGFILES := \
	$(shell find $(CONFIG_PATH) -depth \
		-name \*.config -print)

override CONFIGFILES := $(firstword $(CONFIGFILES))

export configfiles_mk := \
	$(patsubst $(abspath ./%),$(CONFIGFILES_CACHE_DIR)/%.mk, \
		$(abspath $(CONFIGFILES)))

$(MAKECMDGOALS) : $(configfiles_mk)
	@$(MAKE) -f mk/main.mk MAKEFILES='$(all_mk_files) $(mybuild_model_mk) $<' $@

.DELETE_ON_ERROR:

.PHONY :$(configfiles_mk)

$(configfiles_mk) : mk/load3.mk
$(configfiles_mk) : mk/script/mk-persist.mk
$(configfiles_mk) : $(CONFIGFILES_CACHE_DIR)/%.mk : %
	@echo ' CONFIGFILE $<'
	@SCOPE=`echo '$<' | sum | cut -f 1 -d ' '`; \
	mkdir -p $(@D) && \
	$(MAKE) -f mk/script/mk-persist.mk MAKEFILES='$(mk_mybuild_configfile)' \
		PERSIST_OBJECTS='$$(call new,ConfigFileResource,$<)' \
		ALLOC_SCOPE="r$$SCOPE" > $@ && \
	echo '$$(lastword $$(MAKEFILE_LIST)) := '".obj1r$$SCOPE" >> $@

