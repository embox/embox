
PKG_NAME := pjproject
PKG_VER  := 2.2.1

PKG_SOURCES := http://www.pjsip.org/release/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := 6ed4bb7750c827dc1d881e209a3b62db

include $(EXTBLD_LIB)

PKG_PATCHES := pjproject.patch \
	simpleua_default_loglevel.patch \
	mutex_loglevel_increase.patch \
	kmalloc.patch

DISABLE_FEATURES := \
	l16-codec   \
	ilbc-codec  \
	speex-codec \
	speex-aec   \
	gsm-codec   \
	g722-codec  \
	g7221-codec \
	#g711-codec

PJPROJECT_ROOT := $(ROOT_DIR)/third-party/pjproject
PJSIP_CPPFLAGS := -I$(PJPROJECT_ROOT)/include $(EMBOX_DEPS_CPPFLAGS) -I$(SRC_DIR)/compat/cxx/include 
PJSIP_CPPFLAGS += -I$(SRC_DIR)/compat/libc/include -I$(SRC_DIR)/compat/posix/include
BUILD_ROOT := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_ROOT) && ( \
		./aconfigure \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
			CPPFLAGS="$(PJSIP_CPPFLAGS)" \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=/ \
			$(DISABLE_FEATURES:%=--disable-%) \
			--with-external-pa; \
		echo export CFLAGS+="$(PJSIP_CPPFLAGS)" > user.mak; \
		echo export CXXFLAGS+="$(EMBOX_CXXFLAGS) $(PJSIP_CPPFLAGS)" >> user.mak; \
	)
	cp ./config_site.h $(BUILD_ROOT)/pjlib/include/pj/config_site.h
	touch $@

$(BUILD) :
	cd $(BUILD_ROOT) && ( \
		make -j1 dep; \
		make -j1 MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	for f in $(BUILD_ROOT)/pjsip-apps/bin/samples/$(AUTOCONF_TARGET_TRIPLET)/*; do \
       		cp $$f $(PKG_INSTALL_DIR)/$$(basename $$f).o; \
	done
	for f in $(BUILD_ROOT)/pjsip-apps/bin/*-$(AUTOCONF_TARGET_TRIPLET); do \
		fn=$$(basename $$f); \
		cp $$f $(PKG_INSTALL_DIR)/$${fn%-$(AUTOCONF_TARGET_TRIPLET)}.o; \
	done
	touch $@
