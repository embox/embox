#
# Embox main Makefile.
#

include mk/core/common.mk
include mk/util/wildcard.mk
include mk/variables.mk
include mk/version.mk

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
	@CONFPATH=`readlink conf` ; if [ -z $${CONFPATH} ] ; then CONFPATH="conf" ; fi ; \
	echo ">>> Configuration in: $${CONFPATH}/ <<<" ; echo
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

.PHONY : distgen dg
dg : distgen
distgen :
	+@$(make_mybuild) $@

define help-distgen
Usage: $(MAKE) distgen-<template>
   Or: $(MAKE) distgen
   Or: $(MAKE) dg

  Generates build environment for the given <template> (if any)
  or the current active configuration. The environment is generated inside
  DIST_DIR and ready for self-sustained builds.

  Note that in order to use simple form (with no template spesified),
  you have to configure the project first. See configuration targets.
endef # distgen

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
PLATFORM_LABEL:=platform/
PROJECT_LABEL:=project/
EXT_PROJECT_LABEL:=ext_project/
template_name=$(patsubst $(TEMPLATES_DIR)/%,%,$1)
platform_template_name=$(patsubst $(PLATFORM_DIR)/%,$(PLATFORM_LABEL)%,\
		       $(subst $(SUBPLATFORM_TEMPLATE_DIR),,$1))
project_template_name=$(patsubst $(PROJECT_DIR)/%,$(PROJECT_LABEL)%,\
		       $(subst $(SUBPLATFORM_TEMPLATE_DIR),,$1))
ext_project_template_name=$(patsubst $(EXT_PROJECT_DIR)/%,$(EXT_PROJECT_LABEL)%,\
		       $(subst $(SUBPLATFORM_TEMPLATE_DIR),,$1))

template_item=$(foreach t,$(patsubst %/build.conf,%,$2),$(call $1_name,$t)|$t)

__templates := \
	$(call template_item,template, $(call r-wildcard,$(TEMPLATES_DIR)/**/build.conf)) \
	$(call template_item,platform_template, $(call r-wildcard,$(PLATFORM_DIR)/**/build.conf)) \
	$(call template_item,project_template, $(wildcard $(addsuffix /*/build.conf,$(wildcard $(PROJECT_DIR)/*/templates)))) \
	$(call template_item,ext_project_template, $(wildcard $(addsuffix /*/build.conf,$(wildcard $(EXT_PROJECT_DIR)/*/templates))))

templates:=$(foreach t,$(__templates),$(firstword $(subst |, ,$t)))

template_name2dir=$(subst $1|,,$(filter $1|%,$(__templates)))
template_dir2name=$(subst |$1,,$(filter %|$1,$(__templates)))

# build-<template>
# Reruns Make with overridden CONF_DIR
.PHONY : $(templates:%=build-%)
$(templates:%=build-%) : build-% :
	+@$(make_mybuild) CONF_DIR=$(call template_name2dir,$*) build

.PHONY : $(templates:%=buildgen-%)
$(templates:%=buildgen-%) : buildgen-% :
	+@$(make_mybuild) CONF_DIR=$(call template_name2dir,$*) buildgen

.PHONY : $(templates:%=distgen-%)
$(templates:%=distgen-%) : distgen-% :
	+@$(make_mybuild) CONF_DIR=$(call template_name2dir,$*) distgen

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
$(templates:%=confload-%) : confload-% :
	@$(RM) -fR $(CONF_DIR)   #if this is a link, removes link only
	@$(MKDIR) $(CONF_DIR)
	@$(CP) -fR $(call template_name2dir,$*)/* $(CONF_DIR)
	@$(RM) -r $(ROOT_DIR)/build
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
# Disassembly
#

.PHONY : disasm
disasm :
	+@$(make_mybuild) build DISASSEMBLY=y

define help-disasm
Usage: $(MAKE) disasm
  Disassembly image file
endef # disasm

.PHONY : piggy
piggy :
	+@$(make_mybuild) build PIGGY=y

define help-piggy
Usage: $(MAKE) piggy
  Make `piggy' image
endef # piggy

#
# External project targets
#

.PHONY : ext_conf
ext_conf:
	@$(RM) $(EXT_PROJECT_DIR)
	@ln -s $(EXT_PROJECT_PATH) $(EXT_PROJECT_DIR)
	@$(ECHO) "External project path: "$(EXT_PROJECT_PATH)

#
# Cleaning targets.
#

.PHONY : clean
c : clean
clean : #clean regardless build folder as symlink or not
	@$(RM) -r $(ROOT_DIR)/build/*
	@$(RM) -r $(DIST_DIR)/*

define help-clean
Usage: $(MAKE) clean
   Or: $(MAKE) c

  Remove most build artifacts (image, libraries, objects, etc.) #TODO Usecase?
endef # clean

.PHONY : confclean
confclean :
	@$(RM) -r $(CONF_DIR)/*
	@#executes effectively clean on static build folder; not on symlink build 'folder'
	@$(RM) -r $(ROOT_DIR)/build
	@$(RM) -r $(DIST_DIR)

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
Embox version $(EMBOX_VERSION).

Building targets:
  all (a)        - Default build target, alias to '$(MAKE) build'
  build (b)      - Build the current active configuration
  build-<t>      - Build a given configuration template
  buildgen (bg)  - Unconditionally regenerate build environment
  buildgen-<t>   - Prepare build environment for a given template
  distgen (dg)   - Generate self-sustained build environment for distribution
  distgen-<t>    - Prepare distribution for a given template
  menubuild (mb) - Interactively select a configuration to build a menu based
                   program (requires 'dialog')
  xbuild (xb)    - Interactively select a configuration to build using GTK
                   client (requires 'Xdialog')
  disasm         - Build and make disassembly of image
  piggy          - Build and prepare `piggy image'

Configuration targets:
  confload       - List available configuration templates
  confload-<t>   - Load a configuration from template <t>
  menuconfig (mc) - Interactively select a configuration using a menu based
                   program (requires 'dialog')
  xconfig (xc)    - Interactively select a configuration using GTK client
                   (requires 'Xdialog')
  ext_conf        - Add external project (folder) to be used together with Embox.
                    This folder can include one or more different sub-projects folders
                    with theirs own Embox modules, templates.
                    Example: make ext_conf EXT_PROJECT_PATH=<your projects path>

Cleaning targets:
  clean (c)      - Remove most build artifacts (image, libraries, objects, etc.)
  confclean      - Remove loaded configuration (requires redo of configuration setup/load)
		   Also executes clean when the build folder is static (not a symlink)
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
	distgen \
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
	distclean \
	disasm \
	piggy
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

#
# External build lock
#

module_extbld_rmk_target_pat := $(MKGEN_DIR)/%.__extbld-target
module_path = $(subst .,/,$*)
module_lock = $(patsubst %,$(value module_extbld_rmk_target_pat),$(module_path))

.PHONY : extbld-lock
extbld-lock-% :
	@touch $(module_lock)

define help-extbld-lock
Usage: $(MAKE) extbld-lock-qualified.module.name

  Locks external build module for rebuilding. If you want to rebuild it,
  call extbld-unlock.

endef # extbld-lock

.PHONY : extbld-unlock
extbld-unlock-% :
	@-rm $(module_lock)

define help-extbld-unlock
Usage: $(MAKE) extbld-unlock-qualified.module.name

  Unlocks external build module for rebuilding.

endef # extbld-unlock
