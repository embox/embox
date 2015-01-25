#
# Directories.
#

export ROOT_DIR       := .
export EMBOX_ROOT_DIR := $(abspath $(ROOT_DIR))

export CONF_DIR        = $(ROOT_DIR)/conf
export TEMPLATES_DIR   = $(ROOT_DIR)/templates

export SRC_DIR         = $(ROOT_DIR)/src
export THIRDPARTY_DIR  = $(ROOT_DIR)/third-party
export PLATFORM_DIR    = $(ROOT_DIR)/platform
export SUBPLATFORM_TEMPLATE_DIR  = templates/

export BUILD_DIR       = $(ROOT_DIR)/build/base
export EXTERNAL_BUILD_DIR  = $(ROOT_DIR)/build/extbld

export DOC_DIR         = $(ROOT_DIR)/build/doc

export BIN_DIR         = $(BUILD_DIR)/bin
export OBJ_DIR         = $(BUILD_DIR)/obj

export DIST_DIR        = build
export DIST_BASE_DIR   = $(DIST_DIR)/base
export GEN_DIR         = $(DIST_BASE_DIR)/gen
export SRCGEN_DIR      = $(GEN_DIR)
export MKGEN_DIR       = $(GEN_DIR)
export AUTOCONF_DIR    = $(GEN_DIR)

export ROOTFS_DIR      = $(OBJ_DIR)/rootfs
export ROOTFS_IMAGE    = $(OBJ_DIR)/rootfs.cpio
export USER_ROOTFS_DIR = $(CONF_DIR)/rootfs
export DOT_DIR         = $(DOC_DIR)
export DOCS_OUT_DIR    = $(DOC_DIR)

export CACHE_DIR       = mk/.cache

export EMBOX_DIST_BASE_DIR := $(abspath $(DIST_BASE_DIR))
export EXTBLD_LIB      = $(abspath $(ROOT_DIR))/mk/extbld/lib.mk
export EMBOX_GCC_ENV   = $(abspath $(MKGEN_DIR))/embox_gcc_env.sh

export ANNOTATION_HANDLERS  = mk/mybuild/annotation_handlers

#
# Tools.
#

export RM     := rm -f
export CP     := cp
export MV     := mv -f
export PRINTF := printf
export MKDIR  := mkdir -p
export LN     := ln -s
export MD5    := $(shell for i in md5 md5sum; do type $$i >/dev/null 2>&1 && echo $$i && break; done)
export CPIO   := $(shell for i in gcpio cpio; do type $$i >/dev/null 2>&1 && echo $$i && break; done)
export AWK    := $(shell for i in gawk awk nawk mawk; do type $$i >/dev/null 2>&1 && echo $$i && break; done)
export TSORT  := tsort
export TAC    := $(if $(shell which tac),tac,tail -rq)
export SEQ    := seq -w
export ECHO   := $(shell which echo) #force using feature-rich echo instead of shell builtin
