
include $(EXTBLD_DEFINES)

PKG_NAME := mbedtls
PKG_VER  := 2.16.12
PKG_SOURCES :=  https://github.com/Mbed-TLS/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5 := f3a7b041c43b35c883632a1773bf61a6

PKG_PATCHES := Makefile.patch
PKG_PATCHES += ssl_server2.patch
PKG_PATCHES += timing.patch

export EMBOX_CFLAGS
export EMBOX_LDFLAGS
export SRC_DIR
export EMBOX_GCC
export PKG_INSTALL_DIR

$(info EMBOX_CFLAGS: $(EMBOX_CFLAGS))
$(info EMBOX_LDLAGS: $(EMBOX_LDFLAGS))
$(info EMBOX_ARCH:   $(EMBOX_ARCH))

CONFIG_NAME := $(call option_get,STRING,tls_config)

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full

$(BUILD) :
	cp config/$(CONFIG_NAME).h $(PKG_SOURCE_DIR)/include/mbedtls/config.h
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' lib; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install; \
	)
	touch $@
