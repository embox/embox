
PKG_NAME := pjproject
PKG_VER  := 6470fb9e4bd9baa14d50f754dc7f945e7a76c828

PKG_SOURCES := https://github.com/pjsip/pjproject/archive/$(PKG_VER).zip
PKG_MD5     := ec8e45c13d6b8a578612dabf98382141

PKG_PATCHES := pjproject.patch \
               tests.patch

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

ifeq ($(PJSIP_ENABLE_CXX),false)
DISABLE_FEATURES += pjsua2
endif

BUILD_ROOT  := $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)
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
	if [ -f $(CONF_DIR)/pjsip/config_site.h ]; then \
		cp $(CONF_DIR)/pjsip/config_site.h $(BUILD_ROOT)/pjlib/include/pj/config_site.h; \
	else \
		cp ./config_site.h $(BUILD_ROOT)/pjlib/include/pj/config_site.h; \
	fi
	touch $@

$(BUILD) :
	cd $(BUILD_ROOT) && ( \
		$(MAKE) dep; \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
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
	# Install tests
	mkdir $(PJSIP_INSTALL_DIR)/pjlib-test || true
	cp -r $(BUILD_ROOT)/pjlib/build/output/pjlib-test-$(AUTOCONF_TARGET_TRIPLET)/*.o \
		$(PJSIP_INSTALL_DIR)/pjlib-test
	mkdir $(PJSIP_INSTALL_DIR)/pjlib-util-test || true
	cp -r $(BUILD_ROOT)/pjlib-util/build/output/pjlib-util-test-$(AUTOCONF_TARGET_TRIPLET)/*.o \
		$(PJSIP_INSTALL_DIR)/pjlib-util-test
	mkdir $(PJSIP_INSTALL_DIR)/pjmedia-test || true
	cp -r $(BUILD_ROOT)/pjmedia/build/output/pjmedia-test-$(AUTOCONF_TARGET_TRIPLET)/*.o \
		$(PJSIP_INSTALL_DIR)/pjmedia-test
	cp -r $(BUILD_ROOT)/pjmedia/build/Jbtest.dat \
		$(PJSIP_INSTALL_DIR)/pjmedia-test
	mkdir $(PJSIP_INSTALL_DIR)/pjnath-test || true
	cp -r $(BUILD_ROOT)/pjnath/build/output/pjnath-test-$(AUTOCONF_TARGET_TRIPLET)/*.o \
		$(PJSIP_INSTALL_DIR)/pjnath-test
	mkdir $(PJSIP_INSTALL_DIR)/pjsip-test || true
	cp -r $(BUILD_ROOT)/pjsip/build/output/pjsip-test-$(AUTOCONF_TARGET_TRIPLET)/*.o \
		$(PJSIP_INSTALL_DIR)/pjsip-test
	touch $@
