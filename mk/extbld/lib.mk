
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set)
endif

ifeq ($(BUILD_DIR),)
$(error BUILD_DIR is not set)
endif

.PHONY : all
all:

$(BUILD_DIR) :
	mkdir -p $@

PKG_INSTALL_DIR := $(BUILD_DIR)/install

sources          := $(notdir $(PKG_SOURCES))
sources_download := $(PKG_SOURCES)
sources_extract  := $(filter %.tar.gz %.tar.bz %tgz %tbz,$(sources))

DOWNLOAD_DIR   := $(ROOT_DIR)/download
DOWNLOAD     := $(BUILD_DIR)/.downloaded
$(DOWNLOAD): | $(BUILD_DIR)
	[ ! -z $(sources_download) ] && wget -c -P $(DOWNLOAD_DIR) $(sources_download)
	touch $@

EXTRACT   := $(BUILD_DIR)/.extracted
$(EXTRACT): | $(BUILD_DIR) $(DOWNLOAD)
	for i in $(sources_extract); do \
		tar -C $(BUILD_DIR) -axf $(DOWNLOAD_DIR)/$$i; \
	done
	touch $@

PATCH     := $(BUILD_DIR)/.patched
$(PATCH): $(PKG_PATCHES)
$(PATCH): | $(BUILD_DIR) $(EXTRACT)
	for i in $(PKG_PATCHES); do \
		patch -d $(BUILD_DIR) -p0 < $$PWD/$$i; \
	done
	touch $@

CONFIGURE  := $(BUILD_DIR)/.configured
$(CONFIGURE): | $(BUILD_DIR) $(PATCH)

BUILD  := $(BUILD_DIR)/.builded
$(BUILD): $(CONFIGURE) | $(BUILD_DIR)

INSTALL  := $(BUILD_DIR)/.installed
$(INSTALL): $(BUILD) | $(BUILD_DIR) $(PKG_INSTALL_DIR)

all: $(INSTALL)

AUTOCONF_TARGET_TRIPLET=i386-unknown-none
