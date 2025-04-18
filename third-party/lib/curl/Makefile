
PKG_NAME := curl
PKG_VER  := 7.61.1

PKG_SOURCES := https://curl.haxx.se/download/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 4762f41ef0dba751d1c7a3060a1c6ec6

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		CC=$(EMBOX_GCC) ./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
		--disable-shared --enable-static --prefix=/tmp/curl --disable-ldap --disable-sspi \
		prefix=$(PKG_INSTALL_DIR) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
		cp lib/.libs/libcurl.a ../install; \
	)
	touch $@
