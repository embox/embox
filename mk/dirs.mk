
ROOT_DIR       := .

MK_DIR         := $(ROOT_DIR)/mk
SCRIPTS_DIR    := $(ROOT_DIR)/scripts
PROJECTS_DIR   := $(ROOT_DIR)/templates
THIRDPARTY_DIR := $(ROOT_DIR)/third-party
PLATFORM_DIR   := $(ROOT_DIR)/platform
SRC_DIR        := $(ROOT_DIR)/src
DOCS_DIR       := $(ROOT_DIR)/docs

CONF_DIR       := $(ROOT_DIR)/conf

BASE_CONF_DIR  := $(ROOT_DIR)/conf
PATCH_CONF_DIR := $(ROOT_DIR)/conf/$(PATCH_NAME)

BACKUP_DIR     := $(ROOT_DIR)/conf/backup~

BUILD_DIR     := $(ROOT_DIR)/build/$(if $(PATCH_NAME),patch_$(PATCH_NAME),base)

EMBUILD_DIR    := $(MK_DIR)/embuild

BIN_DIR        := $(BUILD_DIR)/bin
OBJ_DIR        := $(BUILD_DIR)/obj
LIB_DIR        := $(BUILD_DIR)/lib
DOT_DIR        := $(BUILD_DIR)/dot
DOCS_OUT_DIR   := $(BUILD_DIR)/docs
CODEGEN_DIR    := $(BUILD_DIR)/codegen
AUTOCONF_DIR   := $(CODEGEN_DIR)
ROOTFS_DIR     := $(BUILD_DIR)/rootfs/
ROOTFS_IMAGE   := $(BUILD_DIR)/rootfs.cpio

EM_DIR         := $(BUILD_DIR)/em
