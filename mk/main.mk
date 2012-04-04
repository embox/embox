#
# Embox main Makefile.
#

include mk/dirs.mk

export RM     := rm -f
export CP     := cp
export PRINTF := printf
export SHELL  := bash
export MKDIR  := mkdir -p
export LN     := ln -s
export PS1    :=

include mk/util/wildcard.mk

.PHONY: clean config xconfig menuconfig conf_update

BUILD_TARGETS := all dot docsgen mkclean

.PHONY: $(BUILD_TARGETS)

$(BUILD_TARGETS):
	@$(MAKE) -f mk/load.mk $@

#
# Configuration related stuff:
#
#   confload-<TEMPLATE>
#   m menuconfig
#   x xconfig
#

# Assuming that we have 'build.conf' in every template.
TEMPLATES := \
	$(sort $(patsubst $(TEMPLATES_DIR)/%/build.conf,%,\
		$(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf)))

$(TEMPLATES:%=confload-%) : confload-% : confclean
	@$(MKDIR) $(CONF_DIR)
	@$(CP) -fR -t $(CONF_DIR) $(TEMPLATES_DIR)/$*/*
	@echo 'Config complete'

# Dialog-based interactive template selections.
m menuconfig : DIALOG := dialog
x xconfig    : DIALOG := Xdialog

m menuconfig \
x xconfig :
	@TEMPLATE=`$(DIALOG) --stdout \
		--backtitle "Configuration template selection" \
		--menu "Select template to load:" 20 40 20 \
		$(TEMPLATES:%=% "" )` \
	&& $(MAKE) confload-$$TEMPLATE

# Old-style configuration.
config :
	@echo '"make config" is considered obsolete and will be removed soon.'
	@echo 'Use "make confload-<TEMPLATE>" instead.'
	$(MAKE) confload-$(PROJECT)/$(PROFILE)

#
# Cleaning targets:
#
#   c clean
#   confclean
#   distclean
#

c clean:
	@$(RM) -r $(ROOT_DIR)/build

confclean : clean
	@$(RM) -r $(CONF_DIR)

distclean: clean confclean mkclean

