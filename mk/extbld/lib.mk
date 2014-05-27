
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set)
endif

ifeq ($(BUILD_DIR),)
$(error BUILD_DIR is not set)
endif

.PHONY : all download extract patch configure build install
all : download extract patch configure build install

PKG_INSTALL_DIR := $(BUILD_DIR)/install
DOWNLOAD_DIR    := $(ROOT_DIR)/download

$(DOWNLOAD_DIR) $(BUILD_DIR) $(PKG_INSTALL_DIR):
	mkdir -p $@

sources_git      := $(filter %.git,$(PKG_SOURCES))
sources_download := $(filter-out %.git,$(PKG_SOURCES))
sources_extract  := $(filter %.tar.gz %.tar.bz2 %tgz %tbz %zip,$(notdir $(sources_download)))

null :=
space := ${null} ${null}
DOWNLOAD  := $(BUILD_DIR)/.downloaded
download : $(DOWNLOAD)
$(DOWNLOAD): | $(DOWNLOAD_DIR)
	$(foreach d,$(sources_download), \
		if [ ! -f $(DOWNLOAD_DIR)/$(notdir $d) ]; then \
			wget -P $(DOWNLOAD_DIR) $d; \
		fi;)
	$(foreach g,$(sources_git), \
		if [ ! -d $(DOWNLOAD_DIR)/$(basename $(notdir $g)) ]; then \
			cd $(DOWNLOAD_DIR); \
			git clone $g; \
		fi;)
	[ `echo $(PKG_SOURCES) | wc -w` -eq `echo $(PKG_MD5) | wc -w` ] || (echo "different number of sources and MD5" && false)
	$(if $(filter-out -,$(PKG_MD5)), \
		cd $(DOWNLOAD_DIR); \
		echo "$(subst ${space},\n,$(filter-out -\t%,$(join $(addsuffix \t,$(PKG_MD5)),$(notdir $(PKG_SOURCES)))))" | md5sum -c --strict; \
		fi)
	touch $@

EXTRACT  := $(BUILD_DIR)/.extracted
extract : $(EXTRACT)
$(EXTRACT): | $(DOWNLOAD_DIR) $(BUILD_DIR)
	$(foreach i,$(sources_extract),\
		$(if $(filter %zip,$i),unzip $(DOWNLOAD_DIR)/$i -d $(BUILD_DIR),\
			tar -C $(BUILD_DIR) -axf $(DOWNLOAD_DIR)/$i);)
	touch $@

PATCH  := $(BUILD_DIR)/.patched
patch : $(PATCH)
PKG_PATCHES ?=
$(PATCH): $(PKG_PATCHES) | $(BUILD_DIR)
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
