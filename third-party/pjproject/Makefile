
PKG_NAME := pjproject
PKG_VER  := 2.10

PKG_SOURCES := https://github.com/pjsip/pjproject/archive/$(PKG_VER).tar.gz
PKG_MD5     := 13e5c418008ae46c4ce0c1e27cdfe9b5

include $(EXTBLD_LIB)

PKG_PATCHES := pjproject-$(PKG_VER).patch \
	sha256_error_fix-$(PKG_VER).patch \
	addr_resolv_sock-$(PKG_VER).patch

ifeq ($(PJSIP_ENABLE_CXX),false)
PKG_PATCHES	+= pjsua2_disable-$(PKG_VER).patch
endif

DISABLE_FEATURES := \
	l16-codec   \
	ilbc-codec  \
	speex-codec \
	speex-aec   \
	gsm-codec   \
	g722-codec  \
	g7221-codec \
	libyuv \
	libwebrtc \
	#g711-codec

BUILD_ROOT  := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)
PJSIP_INSTALL_DIR := $(EXTERNAL_BUILD_DIR)/third_party/pjproject/core/install

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_ROOT) && ( \
		./configure \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=$(PJSIP_INSTALL_DIR) \
			$(DISABLE_FEATURES:%=--disable-%) \
			--with-external-pa; \
	)
	cp ./config_site.h $(BUILD_ROOT)/pjlib/include/pj/config_site.h
	touch $@

$(BUILD) :
	cd $(BUILD_ROOT) && ( \
		$(MAKE) -j1 dep; \
		$(MAKE) -j1 MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(BUILD_ROOT) && $(MAKE) install
	# Remove AUTOCONF_TARGET_TRIPLET from file names to use them in Mybuild
	for f in $(PJSIP_INSTALL_DIR)/lib/*-$(AUTOCONF_TARGET_TRIPLET).a; do \
		fn=$$(basename $$f); \
		cp $$f $(PJSIP_INSTALL_DIR)/lib/$${fn%-$(AUTOCONF_TARGET_TRIPLET).a}.a; \
	done
	# Copy binaries and
	# remove AUTOCONF_TARGET_TRIPLET from file names to use them in Mybuild
	for f in $(BUILD_ROOT)/pjsip-apps/bin/samples/$(AUTOCONF_TARGET_TRIPLET)/*; do \
		cp $$f $(PJSIP_INSTALL_DIR)/$$(basename $$f).o; \
	done
	for f in $(BUILD_ROOT)/pjsip-apps/bin/*-$(AUTOCONF_TARGET_TRIPLET); do \
		fn=$$(basename $$f); \
		cp $$f $(PJSIP_INSTALL_DIR)/$${fn%-$(AUTOCONF_TARGET_TRIPLET)}.o; \
	done
	touch $@
