
PKG_NAME := dropbear
PKG_VER  := 2018.76

PKG_SOURCES := https://matt.ucc.asn.au/dropbear/releases/$(PKG_NAME)-$(PKG_VER).tar.bz2 \
	https://dropbear.nl/mirror/releases/$(PKG_NAME)-$(PKG_VER).tar.bz2

PKG_MD5     := c3912f7fcdcc57c99937e4a79480d2c2

PKG_PATCHES := dropbear.patch

include $(EXTBLD_LIB)

DROPBEAR_CPPFLAGS += -DLTC_NO_BSWAP

$(CONFIGURE) :
	cp stubs.h $(PKG_SOURCE_DIR)
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			CC=$(EMBOX_GCC) \
			CFLAGS="$(DROPBEAR_CPPFLAGS)" \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=/ \
			--disable-zlib \
			--disable-syslog \
			--disable-lastlog \
			--disable-harden \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) install; \
	)
	cp $(PKG_INSTALL_DIR)/sbin/dropbear $(PKG_INSTALL_DIR)/dropbear.o
	touch $@
