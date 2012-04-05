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

build_targets := all dot docsgen
build_targets_implicit := help-mod-%
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

define help_confload
Usage make confload-<TEMPLATE>

    Loads <TEMPLATE> as config. In contrast with template, config is intended
    for user to modify, adding problem-aspect features to system.

endef

# Dialog-based interactive template selections.
.PHONY : m menuconfig
.PHONY : x xconfig

m menuconfig : DIALOG := dialog
x xconfig    : DIALOG := Xdialog
define help_menuconfig
Usage: make menuconfig

    Displays pseudo-graphic menu with avaibale choises for config loading.

    Requires dialog

endef

define help_xconfig
Usage: make xconfig

    Same as menuconfig, but with GTK support. Displays graphic menu with
    avaibale choises for config loading.

    Requires X11, GTK, Xdialog

endef

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

define help_clean
Usage: make clean

    Remove most build artifacts (image, libraries, objects, etc.) #TODO Usecase?

endef

.PHONY : confclean
confclean : clean
	@$(RM) -r $(CONF_DIR)

define help_confclean
Usage: make confclean

  Cleans config directory, suitable for case, when you need precached Mybuild,
  but no config, for example, when you gives a version to some end customers,
  that will not chagne Mybuild's

endef

.PHONY : cacheclean
cacheclean :
	@$(RM) -r $(CACHE_DIR)
define help_cacheclean
Usage: make cacheclean

  Removes build system cache. This is not intended to use manually,
  but may be usefull in build system developing or when update from repo
  causes broken build

endef

.PHONY : distclean
distclean : clean confclean cacheclean

define help_distclean
Usage: make distclean

  Performs full clean: clean, confclean, distclean. After running this,
  root directory reverts to fresh state, just like after fresh checkout
  or after archive extraction.

endef

#
# Make help and its friends.
#
.PHONY : help
help :
	@$(info $(help_main))#

define help_main
Usage: $(MAKE) [targets]
Mybuild version $(MYBUILD_VERSION).

Configuration targets:
    confload-target: load target config from templates
    menuconfig (m): show all possible template choises to load as config (CLI version)
    xconfig (x): show all possible template choises to load as config (GTK version)

Building targets:
    all: default building target, builds main executable

Cleaning targets:
    clean (c): remove build artefacts
    confclean: remove current config; make it blank
    cacheclean: remove build system cache; causing rereading all Mybuild and configfiles
    distclean: make all cleans; makes pure distribution like one after check-out

endef

help_targets := confload menuconfig xconfig all clean confclean cacheclean distclean
.PHONY : $(help_targets:%=help-%)
$(help_targets:%=help-%) : help-% :
	@$(info $(help_$*))#

#default help section
help-% :
	@echo There is no such help section

