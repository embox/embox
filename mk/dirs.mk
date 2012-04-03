
ROOT_DIR       := .

MK_DIR         := $(ROOT_DIR)/mk
SCRIPTS_DIR    := $(ROOT_DIR)/scripts
PROJECTS_DIR   := $(ROOT_DIR)/templates
TEMPLATES_DIR  := $(ROOT_DIR)/templates
THIRDPARTY_DIR := $(ROOT_DIR)/third-party
PLATFORM_DIR   := $(ROOT_DIR)/platform
SRC_DIR        := $(ROOT_DIR)/src
DOCS_DIR       := $(ROOT_DIR)/docs

CONF_DIR       := $(ROOT_DIR)/conf

BACKUP_DIR     := $(ROOT_DIR)/conf/backup~

# XXX check for hardcoded 'build/base' anywhere. -- Eldar
BUILD_DIR     := $(ROOT_DIR)/build/base

EMBUILD_DIR    := mk/embuild

BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj
LIB_DIR        := $(BUILD_DIR)/lib
DOT_DIR        := $(BUILD_DIR)/dot
DOCS_OUT_DIR   := $(BUILD_DIR)/docs
CODEGEN_DIR    := $(BUILD_DIR)/codegen
AUTOCONF_DIR   := $(CODEGEN_DIR)
ROOTFS_DIR     := $(BUILD_DIR)/rootfs
ROOTFS_IMAGE   := $(BUILD_DIR)/rootfs.cpio

EM_DIR         := $(BUILD_DIR)/em
