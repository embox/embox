#
# Embox main Makefile.
#

include mk/core/common.mk
include mk/util/wildcard.mk

#
# Targets that require Mybuild infrastructure.
#

BUILD_TARGETS := all dot docsgen

.PHONY : $(BUILD_TARGETS)
$(BUILD_TARGETS) :
# Call here prevents sub-make invocation in question mode (-q).
# Used to speed up recent bash-completion.
	@$(call MAKE) -f mk/load.mk $@

#
# Configuration related stuff.
#

# Assuming that we have 'build.conf' in every template.
TEMPLATES := \
	$(sort $(patsubst $(TEMPLATES_DIR)/%/build.conf,%, \
		$(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf)))

# confload-<TEMPLATE>
.PHONY : $(TEMPLATES:%=confload-%)
$(TEMPLATES:%=confload-%) : confload-% : confclean
	@$(MKDIR) $(CONF_DIR)
	@$(CP) -fR -t $(CONF_DIR) $(TEMPLATES_DIR)/$*/*
	@echo 'Config complete'

# Dialog-based interactive template selections.
.PHONY : m menuconfig
.PHONY : x xconfig

m menuconfig : DIALOG := dialog
x xconfig    : DIALOG := Xdialog

m menuconfig \
x xconfig :
	@TEMPLATE=`$(DIALOG) --stdout \
		--backtitle "Configuration template selection" \
		--menu "Select template to load:" 20 40 20 \
		$(TEMPLATES:%=% "" )` \
	&& $(MAKE) confload-$$TEMPLATE

.PHONY : config
# Old-style configuration.
config :
	@echo '"make config" is considered obsolete and will be removed soon.'
	@echo 'Use "make confload-<TEMPLATE>" instead.'
	$(MAKE) confload-$(PROJECT)/$(PROFILE)

#
# Cleaning targets.
#

.PHONY : c clean
c clean :
	@$(RM) -r $(ROOT_DIR)/build

.PHONY : confclean
confclean : clean
	@$(RM) -r $(CONF_DIR)

.PHONY : cacheclean
cacheclean :
	@$(RM) -r $(CACHE_DIR)

.PHONY : distclean
distclean : clean confclean cacheclean

