##
# This file should be included by extbld Makefiles.
#
# This also could generate md5sums for specified extbld Makefile:
# Assuming you are in ROOT_DIR, and you are going to generate md5sums of third_party/smth/, run
# make EXTBLD_LIB=$PWD/mk/extbld/lib.mk ROOT_DIR=$PWD -C third-party/smth BUILD_DIR=$PWD/build/extbld/third_party/smth md5_gen
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
.PHONY : md5_gen

all : download extract patch configure build install

PKG_SOURCE_DIR   ?= $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)
PKG_INSTALL_DIR  := $(BUILD_DIR)/install
DOWNLOAD_BASEDIR := $(ROOT_DIR)/download
ifeq ($(value PKG_DOWNLOADS_SEPARATE),)
DOWNLOAD_DIR     := $(DOWNLOAD_BASEDIR)
else
DOWNLOAD_DIR     := $(DOWNLOAD_BASEDIR)/$(PKG_NAME)
endif

$(DOWNLOAD_DIR) $(BUILD_DIR) $(PKG_INSTALL_DIR):
	mkdir -p $@

PKG_SOURCES ?=
# Name of an archive where package will be saved to.
PKG_ARCHIVE_NAME ?=""
sources_git      := $(filter %.git,$(PKG_SOURCES))
targets_git	  = $(basename $(notdir $1))
sources_download := $(filter-out %.git,$(PKG_SOURCES))
# Return PKG_ARCHIVE_NAME of the specified package in case if
# the url does not contain package name
pkg_archive_name  = $(if $(call eq,$(PKG_ARCHIVE_NAME),""),$(call notdir,$1),$(PKG_ARCHIVE_NAME))
sources_extract  := $(filter %.tar.gz %.tar.bz2 %tgz %tbz %zip,$(call pkg_archive_name,$(sources_download)))

DOWNLOAD  := $(BUILD_DIR)/.downloaded
$(DOWNLOAD): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(foreach d,$(sources_download), \
		if [ ! -f $(DOWNLOAD_DIR)/$(call pkg_archive_name,$d) ]; then \
			cd $(DOWNLOAD_DIR) && (curl -o $(call pkg_archive_name,$d) -k -L '$d'); \
		fi;)
	$(foreach g,$(sources_git), \
		if [ ! -d $(DOWNLOAD_DIR)/$(call targets_git,$g) ]; then \
			cd $(DOWNLOAD_DIR); \
			git clone $g; \
		fi;)
	touch $@

DOWNLOAD_CHECK  := $(BUILD_DIR)/.download_checked
$(DOWNLOAD_CHECK) : $(DOWNLOAD)
	$(if $(call eq,$(words $(PKG_SOURCES)),$(words $(PKG_MD5))),, \
		echo "different number of sources and MD5"; false)
	( cd $(DOWNLOAD_DIR); \
		$(foreach c,$(filter-out %.-,$(join $(PKG_SOURCES),$(addprefix .,$(PKG_MD5)))), \
			$(MD5) $(call pkg_archive_name,$(basename $c)) | grep $(subst .,,$(suffix $c)) 2>&1 >/dev/null ; ) \
	)
	touch $@

download : $(DOWNLOAD) $(DOWNLOAD_CHECK)
md5_gen : $(DOWNLOAD)
	@echo PKG_MD5 := \\
	@$(foreach s,$(PKG_SOURCES),md5=$$(md5sum $(DOWNLOAD_DIR)/$(call pkg_archive_name,$s) 2>/dev/null) && echo -e "\\t$${md5%%  *} \\" || echo -- "-";)
	@echo

EXTRACT  := $(BUILD_DIR)/.extracted
extract : $(EXTRACT)
$(EXTRACT): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(foreach i,$(sources_extract),\
		$(if $(filter %zip,$i),unzip -q $(DOWNLOAD_DIR)/$i -d $(BUILD_DIR),\
			mkdir -p $(BUILD_DIR) && ( cd $(BUILD_DIR); tar -xf $(DOWNLOAD_DIR)/$i) );)
	COPY_FILES="$(addprefix $(DOWNLOAD_DIR)/, \
			$(call targets_git,$(sources_git)) \
			$(filter-out $(sources_extract),$(call pkg_archive_name,$(sources_download))))"; \
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
