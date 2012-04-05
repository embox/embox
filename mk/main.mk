#
# Embox main Makefile.
#

include mk/core/common.mk
include mk/util/wildcard.mk
include mk/help.mk

#
# Targets that require Mybuild infrastructure.
#

BUILD_TARGETS := all dot docsgen
BUILD_TARGET_IMPL := help-mod-%

.PHONY : $(BUILD_TARGETS)
$(BUILD_TARGETS) :
# Call here prevents sub-make invocation in question mode (-q).
# Used to speed up recent bash-completion.
	@$(call MAKE) -f mk/load.mk $@

$(BUILD_TARGET_IMPL) :
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

help-clean_title := Clean
help-clean_desc :=\
Cleans building artifacts such as target, objects, etc
help-clean_usage:= make clean


HELP_SECTIONS := help help-clean
.PHONY: $(HELP_SECTIONS)

.PHONY : confclean
confclean : clean
	@$(RM) -r $(CONF_DIR)

.PHONY : cacheclean
cacheclean :
	@$(RM) -r $(CACHE_DIR)

.PHONY : distclean
distclean : clean confclean cacheclean

help_title:= All
help_desc :=\
Build system supports following make targets (there is also shourtcuts):\
$(call help_pointed_list,\
$(call help_section,Clean targets)\
*clean (c): remove build artefacts\
*confclean: remove current config; make it blank\
*cacheclean: remove build system cache; causing rereading all Mybuild and configfiles\
*distclean: make all cleans; makes pure distribution like one after check-out\
$(call help_section,Configure sections)\
*confload-target: load target config from templates\
*menuconfig (m): show all possible template choises to load as config (CLI version)\
*xconfig (x): show all possible template choises to load as config (GTK version)\
$(call help_section,Documentation)\
*docsgen: Generate doc from source's doxygen\
*dot: Generate ps file with illustration of module packages and dependencies\
$(call help_section,Module Inspection)\
$(\n)See help-mod section\
)
help_usage:= make help-subsection with subsection named above

$(HELP_SECTIONS) :
	$(info $(call help_template,$($@_title),$($@_desc),$($@_usage)))
	@#

#default help section
help-% :
	@echo There is no such help section
