#
# Embox main Makefile (distribution).
#

include mk/core/common.mk
include mk/variables.mk
include mk/version.mk

export BUILD_BASE_DIR := .
export EMBOX_DIST_BASE_DIR := $(abspath $(BUILD_BASE_DIR))

.PHONY : all a
a : all
all: build

define help-all
Usage: $(MAKE) [all]
   Or: $(MAKE) a

  Default build target. It is an alias to '$(MAKE) build'.
endef # all

make_build = $(MAKE) -f mk/build.mk

.PHONY : build b
b : build
build :
	+@$(make_build) $@

define help-build
Usage: $(MAKE) build-<template>
   Or: $(MAKE) build
   Or: $(MAKE) b

  Build the given <template> (if any) or the current active configuration.

  Compile all source files and link objects into main executable
  producing various debug and log info.
endef # build

.PHONY : rootfs
rootfs :
	+@$(make_build) build __REBUILD_ROOTFS=1

define help-rootfs
Usage: $(MAKE) rootfs

  Forces rootfs image to be rebuilt unconditionally.
endef # rootfs


#
# Disassembly
#

.PHONY : disasm
disasm :
	+@$(make_build) build DISASSEMBLY=y

define help-disasm
Usage: $(MAKE) disasm
  Disassembly image file
endef # disasm


#
# Docs
#

.PHONY : docsgen
docsgen :
	+@$(make_mybuild) $@

define help-docsgen
Usage: $(MAKE) docsgen

  Generate documentation from doxygen comments in source files.
endef # docsgen

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

#
# Make help and its friends.
#
.PHONY : help
help :
	@$(info $(help_main))#

define help_main
Usage: $(MAKE) [targets]
Embox version $(EMBOX_VERSION) [distibution].

Building targets:
  all (a)        - Default build target, alias to '$(MAKE) build'
  build (b)      - Build the current active configuration

Documentation targets:
  docsgen        - Denerate documentation from doxygen comments

Cleaning targets:
  clean (c)      - Remove most build artifacts (image, libraries, objects, etc.)

endef

help_entries := \
	all \
	build \
	docsgen \
	clean \
	disasm
help_targets := $(help_entries:%=help-%)
