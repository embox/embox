#
# Directories.
#

export ROOT_DIR                 := $(abspath  .)

export SRC_DIR                  := $(ROOT_DIR)/src
export THIRDPARTY_DIR           := $(ROOT_DIR)/third-party
export PLATFORM_DIR             := $(ROOT_DIR)/platform
export PROJECT_DIR              := $(ROOT_DIR)/project
export EXT_PROJECT_DIR          := $(ROOT_DIR)/ext_project
export BUILD_DIR                := $(ROOT_DIR)/build
export CONF_DIR                 := $(ROOT_DIR)/conf
export TEMPLATES_DIR            := $(ROOT_DIR)/templates
export DIST_INSTALL_DIR         := $(ROOT_DIR)/dist

export BUILD_BASE_DIR           := $(BUILD_DIR)/base
export EXTERNAL_BUILD_DIR       := $(BUILD_DIR)/extbld
export LOADABLE_DIR             := $(BUILD_DIR)/loadable
export TOOLCHAIN_DIR            := $(BUILD_DIR)/toolchain
export DOC_DIR                  := $(BUILD_DIR)/doc
export DIST_DIR                 := $(BUILD_DIR)/dist

export BIN_DIR                  := $(BUILD_BASE_DIR)/bin
export OBJ_DIR                  := $(BUILD_BASE_DIR)/obj
export GEN_DIR                  := $(BUILD_BASE_DIR)/gen
export INCLUDE_INSTALL_DIR      := $(BUILD_BASE_DIR)/include

export DIST_INC_DIR             := $(DIST_DIR)/include
export DIST_LIB_DIR             := $(DIST_DIR)/lib
export DIST_BIN_DIR             := $(DIST_DIR)/bin

export DOT_DIR                  := $(DOC_DIR)
export DOCS_OUT_DIR             := $(DOC_DIR)

export SRCGEN_DIR               := $(GEN_DIR)
export MKGEN_DIR                := $(GEN_DIR)
export AUTOCONF_DIR             := $(GEN_DIR)

export ROOTFS_DIR               := $(OBJ_DIR)/rootfs
export ROOTFS_IMAGE             := $(OBJ_DIR)/rootfs.cpio
export USER_ROOTFS_DIR          := $(CONF_DIR)/rootfs

export CACHE_DIR                := mk/.cache
export SUBPLATFORM_TEMPLATE_DIR := templates/
export ANNOTATION_HANDLERS      := mk/mybuild/annotation_handlers

#
# Tools.
#

# find_tool_impl - finds an utillity to be implementation for requested tool.
# This should be used to select proper program on different systems, like
# 'awk' on linux and 'gawk' on bsd. Returns first utility found.
#
# Example: $(call find_tool_impl,AWK,gawk awk nawk mawk,awk)
#
# Args:
#   1. tool (uppercase label)
#   2. space separated list of utillity names to look at
#   3. default choice
find_tool_impl = $(shell \
for i in $2; do \
	type $$i >/dev/null 2>&1 && \
	echo $$i && \
	exit; \
done; \
printf "%s: Found no implementation for %s tool, tried: %s; taking '%s' as default\n" "$0" "$1" "$2" "$3" >&2; \
echo $3)

export RM     := rm -f
export CP     := cp
export MV     := mv -f
export PRINTF := printf
export MKDIR  := mkdir -p
export LN     := ln -s
export MD5    := $(if $(shell which md5sum 2>/dev/null),md5sum,md5 -q)
export CPIO   := $(call find_tool_impl,CPIO,gcpio gnucpio cpio,cpio)
export AWK    := $(call find_tool_impl,AWK,gawk awk nawk mawk,awk)
export TSORT  := tsort
export TAC    := $(if $(shell which tac 2>/dev/null),tac,tail -rq)
export SEQ    := seq -w
export ECHO   := $(shell which echo) #force using feature-rich echo instead of shell builtin
