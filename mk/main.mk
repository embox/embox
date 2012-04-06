#
# Embox main Makefile.
#

export MYBUILD_VERSION := 0.4

include mk/core/common.mk
include mk/util/wildcard.mk

.PHONY : all
all : build

define help-all
Usage: $(MAKE) [all]

  Default build target. It is an alias to '$(MAKE) build'.
endef # all

#
# Targets that require Mybuild infrastructure.
#

build_targets := build dot docsgen
.PHONY : $(build_targets)
$(build_targets) :
# Call here prevents sub-make invocation in question mode (-q).
# Used to speed up recent bash-completion.
	@$(call MAKE) -f mk/load.mk $@

build_targets_implicit := help-mod-%
.PHONY : $(build_targets_implicit)
$(build_targets_implicit) :
	@$(call MAKE) -f mk/load.mk $@

define help-build
Usage: $(MAKE) build
   Or: $(MAKE) build-<template>

  Build current active configuration or the given <template>.

  Compile all source files and link objects into main executable
  producing various debug and log info.

  Note that in order to use simple form ('$(MAKE) build'), you have to
  configure the project first. See configuration targets.
endef # build

define help-dot
Usage: $(MAKE) dot

  Generate PostScript file with module dependency graph.
endef # dot

define help-docsgen
Usage: $(MAKE) docsgen

  Generate documentation from doxygen comments in source files.
endef # docsgen

define help-mod
Usage: $(MAKE) help-mod-<INFO>

  Print <INFO> info about modules:
  list: list all modules included in build
  <module_name>: show brief informataion about module: dependencies, options,
	source files
endef

#
# Configuration related stuff.
#

# Assuming that we have 'build.conf' in every template.
templates := \
	$(sort $(patsubst $(TEMPLATES_DIR)/%/build.conf,%, \
		$(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf)))


# build-<template>
.PHONY : $(templates:%=build-%)
$(templates:%=build-%) : build-% :
	$(call MAKE) CONF_DIR=$(TEMPLATES_DIR)/$* build


.PHONY : confload
confload :
	@$(info $(confload_list)$(\n))#

ifdef templates
define confload_list
List of available templates$:$(subst $(\s),$(\n)  , $(templates))

Use '$(MAKE) confload-<template>' to load one.
endef
else
confload_list := No templates are available.
endif # templates

# confload-<template>
.PHONY : $(templates:%=confload-%)
$(templates:%=confload-%) : confload-% : confclean
	@$(MKDIR) $(CONF_DIR)
	@$(CP) -fR -t $(CONF_DIR) $(TEMPLATES_DIR)/$*/*
	@echo 'Config complete'

define help-confload
Usage: $(MAKE) confload-<template>
   Or: $(MAKE) confload

  Load <template> as a new configuration or list available templates.

  Template is a configuration stub with some reasonable pre-defined defaults.
  Unlike regular configuration, template is not intended to be modified
  directly. Instead, it is loaded as a new configuration into conf/ directory,
  where you can edit it for your needs.
endef # confload

# Dialog-based interactive template selections.
.PHONY : m menuconfig
.PHONY : x xconfig

m menuconfig menubuild : DIALOG := dialog
x xconfig    xbuild: DIALOG := Xdialog

m menuconfig \
x xconfig :
	@TEMPLATE=`$(DIALOG) --stdout \
		--backtitle "Configuration template selection" \
		--menu "Select template to build:" 20 40 20 \
		$(templates:%=% "" )` \
	&& $(MAKE) confload-$$TEMPLATE

menubuild \
xbuild :
	@TEMPLATE=`$(DIALOG) --stdout \
		--backtitle "Configuration template selection" \
		--menu "Select template to build:" 20 40 20 \
		$(templates:%=% "" )` \
	&& $(MAKE) build-$$TEMPLATE

define help-menubuild
Usage: make menubuild

  Same as menuconfig, but for building: display pseudo-graphic menu with template listing
  to immediatly build selected one.
endef

define help-menuconfig
Usage: $(MAKE) menuconfig

  Displays pseudo-graphic menu listing templates available to be loaded
  as a configuration.

  Requires 'dialog'.
endef # menuconfig

define help-xconfig
Usage: $(MAKE) xconfig

  Same as menuconfig, but with GTK support. Displays graphic menu with
  avaibale choises for config loading.

  Requires X11, Xdialog.
endef # xconfig

define help-xbuild
Usage: make xbuild

  Same as xconfig, but for building, display menu with template listing
  to immediatly build selected one.
endef

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
Usage: $(MAKE) clean
   Or: $(MAKE) c

  Remove most build artifacts (image, libraries, objects, etc.) #TODO Usecase?
endef # clean

.PHONY : confclean
confclean : clean
	@$(RM) -r $(CONF_DIR)

define help-confclean
Usage: $(MAKE) confclean

  Cleans config directory, suitable for case, when you need precached Mybuild,
  but no config, for example, when you gives a version to some end customers,
  that will not change Mybuild files.
endef # confclean

.PHONY : cacheclean
cacheclean :
	@$(RM) -r $(CACHE_DIR)

define help-cacheclean
Usage: $(MAKE) cacheclean

  Removes build system cache. This is not intended to use manually,
  but may be usefull in build system developing or when update from repo
  causes broken build.
endef # cacheclean

.PHONY : distclean
distclean : clean confclean cacheclean

define help-distclean
Usage: $(MAKE) distclean

  Performs full clean: clean, confclean, distclean. After running this,
  root directory reverts to fresh state, just like after fresh checkout
  or after archive extraction.
endef # distclean

#
# Make help and its friends.
#
.PHONY : help
help :
	@$(info $(help_main))#

define help_main
Usage: $(MAKE) [targets]
Mybuild version $(MYBUILD_VERSION).

Building targets:
  all            - Default build target, alias to '$(MAKE) build'
  build          - Build the current active configuration
  build-<t>      - Build a given configuration template
  menubuild      - Interactively select a configuration to build using
                   a menu based program (requires 'dialog')
  xbuild         - Interactively select a configuration to build using
                   GTK client (requires 'Xdialog')

Configuration targets:
  confload       - List available configuration templates
  confload-<t>   - Load a configuration from template <t>
  menuconfig (m) - Interactively select a configuration using a menu based
                   program (requires 'dialog')
  xconfig (x)    - Interactively select a configuration using GTK client
                   (requires 'Xdialog')

Cleaning targets:
  clean (c)      - Remove most build artifacts (image, libraries, objects, etc.)
  confclean      - Remove build artifacts and loaded configuration
  cacheclean     - Flush Mybuild internal cache
  distclean      - clean + confclean + cacheclean

Documentation targets:
  dot            - Generate a picture with module dependency graph
  doxygen        - Denerate documentation from doxygen comments

Module information:
  help-mod       - Various module information. See more there

endef

help_entries := \
	all \
	build \
	menubuild \
	xbuild \
	dot \
	docsgen \
	mod \
	confload \
	menuconfig \
	xconfig \
	clean \
	confclean \
	cacheclean \
	distclean
help_targets := $(help_entries:%=help-%)

# Fixup documentation variables escaping colons (':' -> '$$:').
# This prevents bash-completion to treat lines like 'Building targets:' as
# rules.
$(foreach h,help_main $(help_targets), \
	$(eval define $h$(\n)$(subst :,$$:,$(value $h))$(\n)endef))

.PHONY : $(help_targets)
$(help_targets) :
	@$(info $($@)$(\n))#

# Default help section.
help-% :
	@echo No help section for '$*'

