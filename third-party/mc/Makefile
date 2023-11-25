PKG_NAME := mc
PKG_VER := 4.8.21

PKG_SOURCES := http://ftp.midnight-commander.org/$(PKG_NAME)-$(PKG_VER).tar.xz

PKG_MD5 := 743a421db8a5469bb373874cd6efbcb2

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
	    CC=$(EMBOX_GCC) ./configure \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			CHECK_CFLAGS="-I$(EXTERNAL_BUILD_DIR)/third_party/libs/check/install" \
			CHECK_LIBS="-L$(EXTERNAL_BUILD_DIR)/third_party/libs/check/lib -lcheck" \
			GLIB_CFLAGS="-I$(EXTERNAL_BUILD_DIR)/third_party/libs/glib/install/include/glib-2.0 -I$(EXTERNAL_BUILD_DIR)/third_party/libs/glib/install/lib/glib-2.0/include" \
			GLIB_LIBS="-L$(EXTERNAL_BUILD_DIR)/third_party/libs/glib/install/lib -lglib-2.0" \
			GMODULE_CFLAGS="-I$(EXTERNAL_BUILD_DIR)/third_party/libs/glib/install/include/glib-2.0 -I$(EXTERNAL_BUILD_DIR)/third_party/libs/glib/install/lib/glib-2.0/include" \
			GMODULE_LIBS="-L$(EXTERNAL_BUILD_DIR)/third_party/libs/glib/install/lib/glib-2.0 -lgmodule-2.0" \
			--without-gpm-mouse \
			--without-edit \
			--enable-static=yes \
			--prefix=$(BUILD_DIR)/install \
			--with-screen=ncurses \
			--with-ncurses-includes="$(EXTERNAL_BUILD_DIR)/third_party/libs/ncurses/libncurses/install/include" \
			--with-ncurses-libs="$(EXTERNAL_BUILD_DIR)/third_party/libs/ncurses/libncurses/install/lib" \
			--disable-FEATURE --disable-rpath --disable-largefile --disable-vfs --disable-nls \
	)
	touch $@

$(BUILD) :
	cp -r ./include/* $(INCLUDE_INSTALL_DIR)/
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		cp $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/src/mc $(PKG_INSTALL_DIR)/mc.o; \
	); \
	
	touch $@

