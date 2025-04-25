
PKG_NAME := sqlite-autoconf
PKG_VER  := 3080403

PKG_SOURCES := http://www.sqlite.org/2014/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 0f5459cde43cb269e5120ecd2c671ced

PKG_PATCHES := pkg_patch.txt setpriority.patch

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--disable-largefile \
			--disable-shared \
			--disable-static \
			--disable-threadsafe \
			--disable-dynamic-extensions \
			--enable-readline \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-I$(ROOT_DIR)/third-party/sqlite/include/ \
				-DSQLITE_DEBUG=1 \
				-DSQLITE_ENABLE_PRIORITY"; \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/sqlite3 $(PKG_INSTALL_DIR)/sqlite3.o
	touch $@
