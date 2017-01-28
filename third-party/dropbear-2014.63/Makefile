
PKG_NAME := dropbear
PKG_VER  := 2014.63

PKG_SOURCES := https://matt.ucc.asn.au/dropbear/releases/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := 7066bb9a2da708f3ed06314fdc9c47fd

PKG_PATCHES := dropbear.patch

include $(EXTBLD_LIB)

$(CONFIGURE) :
	cp stubs.h $(PKG_SOURCE_DIR)
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=/ \
			--disable-zlib \
			--disable-syslog \
			--disable-lastlog \
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
