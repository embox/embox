
PKG_NAME := dropbear
PKG_VER  := 2014.63

PKG_SOURCES := https://matt.ucc.asn.au/dropbear/$(PKG_NAME)-$(PKG_VER).tar.bz2

PKG_PATCHES := dropbear.patch

include $(EXTBLD_LIB)

$(CONFIGURE) :
	#exit 1
	cp stubs.h $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			--disable-zlib \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make DESTDIR=$(PKG_INSTALL_DIR) install; \
	)
	cp $(PKG_INSTALL_DIR)/usr/local/sbin/dropbear $(PKG_INSTALL_DIR)/dropbear.o
	cp $(PKG_INSTALL_DIR)/usr/local/bin/dropbearkey $(PKG_INSTALL_DIR)/dropbearkey.o
	touch $@
