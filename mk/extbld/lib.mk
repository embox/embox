
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

PKG_SOURCE_DIR  ?= $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)
PKG_INSTALL_DIR := $(BUILD_DIR)/install
DOWNLOAD_DIR    := $(ROOT_DIR)/download

$(DOWNLOAD_DIR) $(BUILD_DIR) $(PKG_INSTALL_DIR):
	mkdir -p $@

PKG_SOURCES ?=
sources_git      := $(filter %.git,$(PKG_SOURCES))
targets_git	  = $(basename $(notdir $1))
sources_download := $(filter-out %.git,$(PKG_SOURCES))
targets_download  = $(notdir $1)
sources_extract  := $(filter %.tar.gz %.tar.bz2 %tgz %tbz %zip,$(call targets_download,$(sources_download)))

DOWNLOAD  := $(BUILD_DIR)/.downloaded
download : $(DOWNLOAD)
$(DOWNLOAD): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(foreach d,$(sources_download), \
		if [ ! -f $(DOWNLOAD_DIR)/$(call targets_download,$d) ]; then \
			wget -P $(DOWNLOAD_DIR) $d; \
		fi;)
	$(foreach g,$(sources_git), \
		if [ ! -d $(DOWNLOAD_DIR)/$(call targets_git,$g) ]; then \
			cd $(DOWNLOAD_DIR); \
			git clone $g; \
		fi;)
	$(if $(call eq,$(words $(PKG_SOURCES)),$(words $(PKG_MD5))),, \
		echo "different number of sources and MD5"; false)
	( cd $(DOWNLOAD_DIR); \
		$(foreach c,$(filter-out %.-,$(join $(PKG_SOURCES),$(addprefix .,$(PKG_MD5)))), \
			echo "$(subst .,,$(suffix $c))  $(notdir $(basename $c))" | md5sum -c --strict ; ) \
	)

	touch $@

EXTRACT  := $(BUILD_DIR)/.extracted
extract : $(EXTRACT)
$(EXTRACT): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(foreach i,$(sources_extract),\
		$(if $(filter %zip,$i),unzip $(DOWNLOAD_DIR)/$i -d $(BUILD_DIR),\
			tar -C $(BUILD_DIR) -axf $(DOWNLOAD_DIR)/$i);)
	COPY_FILES="$(addprefix $(DOWNLOAD_DIR)/, \
			$(call targets_git,$(sources_git)) \
			$(filter-out $(sources_extract),$(call targets_download,$(sources_download))))"; \
		if [ $$COPY_FILES ]; then \
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

include $(MKGEN_DIR:.%=$(ROOT_DIR)%)/build.mk

ifeq ($(ARCH),x86)
AUTOCONF_ARCH := i386
else
AUTOCONF_ARCH := $(ARCH)
endif

AUTOCONF_TARGET_TRIPLET=$(AUTOCONF_ARCH)-unknown-none
EMBOX_GCC := $(ROOT_DIR)/mk/extbld/arch-embox-gcc
EMBOX_GXX := $(ROOT_DIR)/mk/extbld/arch-embox-g++
