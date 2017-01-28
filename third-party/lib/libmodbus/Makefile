
PKG_NAME := libmodbus
PKG_VER  := 3.0.6

PKG_SOURCES := http://libmodbus.org/releases/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := c80f88b6ca19cabc4ceffc195ca07771

include $(EXTBLD_LIB)

libmodbus_cflags = -I$(ROOT_DIR)/third-party/lib/libmodbus/include/ \

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
