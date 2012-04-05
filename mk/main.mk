#
# Embox main Makefile.
#

export MYBUILD_VERSION := 0.4

include mk/core/common.mk
include mk/util/wildcard.mk
include mk/help.mk

#
# Targets that require Mybuild infrastructure.
#

#####
build_targets += all

help_brief-all = \
	Build the current configuration
help_targets += all

#####
build_targets += dot

help_brief-dot = \
	Generate ps file with illustration of module packages and dependencies
help_targets += dot

#####
build_targets += docsgen

help_brief-docsgen = \
	Generate doc from source's doxygen
help_targets += docsgen

#####
build_targets += help-mod-%

help_brief-help-mod-list = \
	NIY
help_targets += help-mod-list

.PHONY : $(build_targets)
$(build_targets) :
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
#####
.PHONY : c clean
c clean :
	@$(RM) -r $(ROOT_DIR)/build

help_brief-clean = \
	Remove most build artifacts (image, libraries, objects, etc.)
help_alias-clean = \
	c
help_targets += clean

#####
.PHONY : confclean
confclean : clean
	@$(RM) -r $(CONF_DIR)

help_brief-confclean = \
	Remove build artifacts and loaded configuration
help_targets += confclean

#####
.PHONY : cacheclean
cacheclean :
	@$(RM) -r $(CACHE_DIR)

help_brief-cacheclean = \
	Flush Mybuild internal cache
help_targets += cacheclean

#####
.PHONY : distclean
distclean : clean confclean cacheclean

help_brief-distclean = \
	clean + confclean + cacheclean
help_targets += distclean

#
# Make help and its friends.
#
#####
.PHONY : help
help :
	@$(info $(help_main))#

help_brief-help = \
	Print this message
help_alias-help = \
	h
help_targets += help

# Message for 'make help'.
define help_main
Usage: $(MAKE) [targets]
Mybuild version $(MYBUILD_VERSION).

Configuration targets:
    $(call help_brief,confload)

Building targets:
    $(call help_brief,all)

Cleaning targets:
    $(call help_brief,clean)
    $(call help_brief,confclean)
    $(call help_brief,cacheclean)
    $(call help_brief,distclean)

endef


.PHONY : $(help_targets:%=help-%)
$(help_targets:%=help-%) : help-% :
	@$(info $(call help_message,$*))#

# 1. Name of the target.
help_message = \
	$(help_brief)

# 1. Name of the target.
help_brief = \
	$1$(if $(value help_alias-$1), \
				($(subst $(\s),$(,) ,$(strip $(help_alias-$1)))))$(if \
		$(value help_brief-$1), - $(help_brief-$1))

ifeq (0,1)
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
endif
