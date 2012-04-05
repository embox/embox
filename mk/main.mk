#
# Embox main Makefile.
#

export MYBUILD_VERSION := 0.4

include mk/core/common.mk
include mk/util/wildcard.mk

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

$(build_targets_implicit) :
	@$(call MAKE) -f mk/load.mk $@

#
# Configuration related stuff.
#

define help-all
Usage: make [all]

  Stands for default build target. Makes all generated source files, objects,
  main executable, generate various debuggin and logging info.

endef

define help-dot
Usage: make dot

  Generates ps file with module dependencies. Modules are also grouped by package

endef

define help-docsgen
Usage: make docsgen

  Generate documentation from doxygen comments in source files

endef

define help-mod
Usage: make help-mod-<INFO>

  Print <INFO> info about modules:
  list: list all modules included in build
  <module_name>: show brief informataion about module: dependencies, options,
	source files

endef

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

define help-confload
Usage make confload-<TEMPLATE>

  Loads <TEMPLATE> as config. In contrast with template, config is intended
  for user to modify, adding problem-aspect features to system.

endef

# Dialog-based interactive template selections.
.PHONY : m menuconfig
.PHONY : x xconfig

m menuconfig : DIALOG := dialog
x xconfig    : DIALOG := Xdialog
define help-menuconfig
Usage: make menuconfig

  Displays pseudo-graphic menu with avaibale choises for config loading.

  Requires dialog

endef

define help-xconfig
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

define help-clean
Usage: make clean

  Remove most build artifacts (image, libraries, objects, etc.) #TODO Usecase?

endef

.PHONY : confclean
confclean : clean
	@$(RM) -r $(CONF_DIR)

define help-confclean
Usage: make confclean

Cleans config directory, suitable for case, when you need precached Mybuild,
but no config, for example, when you gives a version to some end customers,
that will not chagne Mybuild's

endef

.PHONY : cacheclean
cacheclean :
	@$(RM) -r $(CACHE_DIR)
define help-cacheclean
Usage: make cacheclean

Removes build system cache. This is not intended to use manually,
but may be usefull in build system developing or when update from repo
causes broken build

endef

.PHONY : distclean
distclean : clean confclean cacheclean

define help-distclean
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
	@$(info $(help-main))#

define help-main
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

Document targets:
  dot: generate dot files
  doxygen: generate documentation from doxygen comments

Module information
  help-mod: various module information. See more there

endef

help_targets := confload menuconfig xconfig all clean confclean cacheclean distclean dot \
	doxygen mod
.PHONY : $(help_targets:%=help-%)
$(help_targets:%=help-%) : help-% :
	@$(info $(help-$*))#

#default help section
help-% :
	@echo There is no such help section

