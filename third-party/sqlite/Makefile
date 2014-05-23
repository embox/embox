
PKG_NAME := sqlite
PKG_VER  := autoconf-3080403

PKG_SOURCES := http://www.sqlite.org/2014/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 0f5459cde43cb269e5120ecd2c671ced

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
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
				-DSQLITE_DEBUG=1"; \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/sqlite3 $(PKG_INSTALL_DIR)/sqlite3.o
	touch $@
