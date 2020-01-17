
PKG_NAME := libnpf

PKG_SOURCES := http://download.embox.rocks/$(PKG_NAME).zip
PKG_MD5     := 29d889b6bfd38cb1fce2d4cf55950728

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
