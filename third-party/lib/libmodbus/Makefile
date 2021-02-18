
PKG_NAME := libmodbus
PKG_VER  := 3.1.6

PKG_SOURCES := http://libmodbus.org/releases/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 15c84c1f7fb49502b3efaaa668cfd25e

PKG_PATCHES := accept4_disable.patch

include $(EXTBLD_LIB)

libmodbus_cflags = -UHAVE_ACCEPT4

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		CC=$(EMBOX_GCC) ./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
		prefix=$(PKG_INSTALL_DIR) \
		CFLAGS=$(libmodbus_cflags) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
