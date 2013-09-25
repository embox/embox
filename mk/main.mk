#
# Embox main Makefile.
#

export MYBUILD_VERSION := 0.4

include mk/core/common.mk
include mk/util/wildcard.mk

.PHONY : all a
a : all
all: build

define help-all
Usage: $(MAKE) [all]
   Or: $(MAKE) a

  Default build target. It is an alias to '$(MAKE) build'.
endef # all

#
# Targets that require Mybuild infrastructure.
#

make_mybuild = $(MAKE) -f mk/load.mk

.PHONY : build b
b : build
build :
	+@$(make_mybuild) $@

define help-build
Usage: $(MAKE) build-<template>
   Or: $(MAKE) build
   Or: $(MAKE) b

  Build the given <template> (if any) or the current active configuration.

  Compile all source files and link objects into main executable
  producing various debug and log info.

  Note that in order to use simple form (with no template spesified),
  you have to configure the project first. See configuration targets.
endef # build

.PHONY : buildgen bg
bg : buildgen
buildgen :
	+@$(make_mybuild) $@

define help-buildgen
Usage: $(MAKE) buildgen-<template>
   Or: $(MAKE) buildgen
   Or: $(MAKE) bg

  Generates build environment for the given <template> (if any)
  or the current active configuration.

  Note that in order to use simple form (with no template spesified),
  you have to configure the project first. See configuration targets.
endef # buildgen

.PHONY : rootfs
rootfs :
	+@$(make_mybuild) build __REBUILD_ROOTFS=1

define help-rootfs
Usage: $(MAKE) rootfs

  Forces rootfs image to be rebuilt unconditionally.
endef # rootfs

.PHONY : dot
dot :
	+@$(make_mybuild) $@

define help-dot
Usage: $(MAKE) dot

  Generate PostScript file with module dependency graph.
endef # dot

.PHONY : docsgen
docsgen :
	+@$(make_mybuild) $@

define help-docsgen
Usage: $(MAKE) docsgen

  Generate documentation from doxygen comments in source files.
endef # docsgen

mod-% :
	+@$(make_mybuild) $@

define help-mod
Usage: $(MAKE) mod-<INFO>

  Print <INFO> info about modules:
  list: list all modules included in build
  brief-<module_name>: show brief informataion about module: dependencies, options,
	source files
  include-reason-<module_name>: show dependence subtree desribing why <module_name>
    was included in build
endef

# Assuming that we have 'build.conf' in every template.
templates := \
	$(sort $(patsubst $(TEMPLATES_DIR)/%/build.conf,%, \
		$(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf)))

# build-<template>
# Reruns Make with overridden CONF_DIR
.PHONY : $(templates:%=build-%)
$(templates:%=build-%) : build-% :
	+@$(make_mybuild) CONF_DIR=$(TEMPLATES_DIR)/$* build

.PHONY : $(templates:%=buildgen-%)
$(templates:%=buildgen-%) : buildgen-% :
	+@$(make_mybuild) CONF_DIR=$(TEMPLATES_DIR)/$* buildgen

#
# Configuration related stuff.
#

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

.PHONY : config
# Old-style configuration.
config :
	@echo '"make config" is considered obsolete and will be removed soon.'
	@echo 'Use "make confload-<template>" instead.'
	$(MAKE) confload-$(PROJECT)/$(PROFILE)

#
# Dialog-based interactive template selections.
#

.PHONY : mb menubuild
.PHONY : xb xbuild
.PHONY : mc menuconfig
.PHONY : xc xconfig

mb : menubuild
xb : xbuild
mc : menuconfig
xc : xconfig

menubuild  menuconfig : DIALOG := dialog
xbuild     xconfig    : DIALOG := Xdialog

menubuild  xbuild     : target := build
menuconfig xconfig    : target := confload

menubuild   xbuild    \
menuconfig  xconfig   :
	@TEMPLATE=`$(DIALOG) --stdout \
		--backtitle "Configuration template selection" \
		--clear \
		--menu "Select template to $(target:conf%=%):" 20 40 20 \
		$(templates:%=% "" )` \
	&& $(MAKE) $(target)-$$TEMPLATE

define help-menubuild
Usage: $(MAKE) menubuild
   Or: $(MAKE) mb

  Display pseudo-graphic menu with list of templates, allows you
  to build the selected one immediatelly.

  Requires 'dialog'.
endef # menubuild

define help-xbuild
Usage: $(MAKE) xbuild
   Or: $(MAKE) xb

  Same as menubuild, but uses grapical menu.

  Requires X11, Xdialog.
endef # xbuild

define help-menuconfig
Usage: $(MAKE) menuconfig
   Or: $(MAKE) mc

  Display pseudo-graphic menu listing templates available to be loaded
  as a configuration.

  Requires 'dialog'.
endef # menuconfig

define help-xconfig
Usage: $(MAKE) xconfig
   Or: $(MAKE) xc

  Same as menuconfig, but uses grapical menu.

  Requires X11, Xdialog.
endef # xconfig

#
# Cleaning targets.
#

.PHONY : clean
c : clean
clean :
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
dc : distclean
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
  all (a)        - Default build target, alias to '$(MAKE) build'
  build (b)      - Build the current active configuration
  build-<t>      - Build a given configuration template
  buildgen (bg)  - Unconditionally regenerate build environment
  buildgen-<t>   - Prepare build environment for a given template
  menubuild (mb) - Interactively select a configuration to build a menu based
                   program (requires 'dialog')
  xbuild (xb)    - Interactively select a configuration to build using GTK
                   client (requires 'Xdialog')

Configuration targets:
  confload       - List available configuration templates
  confload-<t>   - Load a configuration from template <t>
  menuconfig (mc) - Interactively select a configuration using a menu based
                   program (requires 'dialog')
  xconfig (xc)    - Interactively select a configuration using GTK client
                   (requires 'Xdialog')

Cleaning targets:
  clean (c)      - Remove most build artifacts (image, libraries, objects, etc.)
  confclean      - Remove build artifacts and loaded configuration
  cacheclean     - Flush Mybuild internal cache
  distclean (dc) - clean + confclean + cacheclean

Documentation targets:
  dot            - Generate a picture with module dependency graph
  docsgen        - Denerate documentation from doxygen comments

Module information:
  mod            - Various module information. See more in help-mod

endef

help_entries := \
	all \
	build \
	buildgen \
	dot \
	docsgen \
	mod \
	confload \
	menubuild \
	xbuild \
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
