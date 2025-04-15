
PKG_NAME := dropbear
PKG_VER  := 2022.83

PKG_SOURCES := https://matt.ucc.asn.au/dropbear/releases/$(PKG_NAME)-$(PKG_VER).tar.bz2 \
	https://dropbear.nl/mirror/releases/$(PKG_NAME)-$(PKG_VER).tar.bz2

#PKG_MD5     := c3912f7fcdcc57c99937e4a79480d2c2
PKG_MD5 := a75a34bcc03cacf71a2db9da3b7c94a5

PKG_PATCHES := dropbear-mpr.patch

DROPBEAR_CPPFLAGS += -DLTC_NO_BSWAP

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cp stubs.h $(PKG_SOURCE_DIR)
	cp embox_drpbr.c $(PKG_SOURCE_DIR)
	cp embox_drpbr.h $(PKG_SOURCE_DIR)
	cp $(PKG_SOURCE_DIR)/default_options.h $(PKG_SOURCE_DIR)/localoptions.h
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
