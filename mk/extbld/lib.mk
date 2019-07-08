##
# This file should be included by extbld Makefiles.
#

ifeq ($(strip $(ROOT_DIR)),)
$(error ROOT_DIR is not set)
endif

ifeq ($(strip $(BUILD_DIR)),)
$(error BUILD_DIR is not set)
endif

include $(ROOT_DIR)/mk/core/common.mk
include $(ROOT_DIR)/mk/core/string.mk

.PHONY : all download extract patch configure build install

all : download extract patch configure build install

PKG_INSTALL_DIR  := $(BUILD_DIR)/install
PKG_SOURCE_DIR   = $(filter-out %/install,$(wildcard $(BUILD_DIR)/*))
DOWNLOAD_BASEDIR := $(ROOT_DIR)/download
ifeq ($(value PKG_DOWNLOADS_SEPARATE),)
DOWNLOAD_DIR     := $(DOWNLOAD_BASEDIR)
else
DOWNLOAD_DIR     := $(DOWNLOAD_BASEDIR)/$(PKG_NAME)
endif

$(DOWNLOAD_DIR) $(BUILD_DIR) $(PKG_INSTALL_DIR):
	mkdir -p $@

PKG_NAME    ?=
PKG_VER     ?=
PKG_SOURCES ?=

sources_git := $(filter %.git,$(PKG_SOURCES))
targets_git = $(basename $(notdir $1))
sources_archive_mirrors := $(filter-out %.git,$(PKG_SOURCES))

pkg_ext ?=
first_url := $(word 1,$(sources_archive_mirrors))
ifneq ($(filter %.tar.gz %.tar.bz %.tar.bz2 %.tar.xz,$(first_url)),)
	pkg_ext := .tar$(suffix $(first_url))
else ifneq ($(filter %.tgz %.tbz %.zip,$(first_url)),)
	pkg_ext := $(suffix $(first_url))
endif

ifneq ($(PKG_VER),)
	pkg_archive_name := $(PKG_NAME)-$(PKG_VER)$(pkg_ext)
else
	pkg_archive_name := $(PKG_NAME)$(pkg_ext)
endif

DOWNLOAD  := $(BUILD_DIR)/.downloaded
$(DOWNLOAD): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(foreach d,$(sources_archive_mirrors), \
		if [ ! -f $(DOWNLOAD_DIR)/$(pkg_archive_name) ] ; then \
			cd $(DOWNLOAD_DIR); \
			curl -o $(pkg_archive_name) -f -k -L '$d' || $(RM) $(pkg_archive_name); \
		fi;) \
	$(foreach g,$(sources_git), \
		if [ ! -d $(DOWNLOAD_DIR)/$(call targets_git,$g) ]; then \
			cd $(DOWNLOAD_DIR); \
			git clone $g; \
		fi;)
	touch $@

DOWNLOAD_CHECK  := $(BUILD_DIR)/.download_checked
$(DOWNLOAD_CHECK) : $(DOWNLOAD)
	cd $(DOWNLOAD_DIR) && ( \
		$(MD5) $(pkg_archive_name) | $(AWK) '{print $$1}' | grep $(PKG_MD5) 2>&1 >/dev/null; \
	)
	touch $@

download : $(DOWNLOAD) $(DOWNLOAD_CHECK)

EXTRACT  := $(BUILD_DIR)/.extracted
extract : $(EXTRACT)
$(EXTRACT): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(if $(filter %zip,$(pkg_ext)), \
		unzip -q $(DOWNLOAD_DIR)/$(pkg_archive_name) -d $(BUILD_DIR), \
		tar -xf $(DOWNLOAD_DIR)/$(pkg_archive_name) -C $(BUILD_DIR))
	COPY_FILES="$(addprefix $(DOWNLOAD_DIR)/, \
			$(call targets_git,$(sources_git)))"; \
		if [ "$$COPY_FILES" ]; then \
			cp -R $$COPY_FILES $(BUILD_DIR); \
		fi
	touch $@

PATCH  := $(BUILD_DIR)/.patched
patch : $(PATCH)
PKG_PATCHES ?=
$(PATCH): $(PKG_PATCHES) | $(BUILD_DIR)
	if [ -d tree ]; then \
		cd tree; \
		cp -l -u -f --parents -t $(PKG_SOURCE_DIR) `find . -not -path '*/.*' -and -type f`; \
		cd ..; \
	fi
	for i in $(PKG_PATCHES); do \
		patch -d $(BUILD_DIR) -p0 < $$PWD/$$i; \
	done
	touch $@

CONFIGURE  := $(BUILD_DIR)/.configured
configure : $(CONFIGURE)
$(CONFIGURE): | $(BUILD_DIR)

BUILD  := $(BUILD_DIR)/.builded
build : $(BUILD)
$(BUILD): | $(BUILD_DIR)

INSTALL  := $(BUILD_DIR)/.installed
install : $(INSTALL)
$(INSTALL): | $(BUILD_DIR) $(PKG_INSTALL_DIR)

# Definitions used by user Makefile

ifneq ($(MKGEN_DIR),)
include $(MKGEN_DIR:.%=$(ROOT_DIR)%)/build.mk
ifeq ($(ARCH),x86)
AUTOCONF_ARCH := i386
else
AUTOCONF_ARCH := $(ARCH)
endif
AUTOCONF_TARGET_TRIPLET=$(AUTOCONF_ARCH)-unknown-none
endif

ifeq ($(COMPILER),clang)
EMBOX_GCC := $(ROOT_DIR)/mk/extbld/arch-embox-clang
else
EMBOX_GCC := $(ROOT_DIR)/mk/extbld/arch-embox-gcc
EMBOX_GXX := $(ROOT_DIR)/mk/extbld/arch-embox-g++
endif
