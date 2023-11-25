PKG_NAME := glib
PKG_VER := 2.57.1

PKG_SOURCES := https://download.gnome.org/sources/glib/2.57/$(PKG_NAME)-$(PKG_VER).tar.xz

PKG_MD5 := e32b090f0c24a6be61479a6e7a3f2a77

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
	    CC=$(EMBOX_GCC) ./configure \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			ZLIB_CFLAGS="-I$(EXTERNAL_BUILD_DIR)/third_party/zlib/libs/install" \
			ZLIB_LIBS="-L$(PKG_INSTALL_DIR) -lz" \
			LIBFFI_CFLAGS="-I$(EXTERNAL_BUILD_DIR)/third_party/libs/libffi/install" \
			LIBFFI_LIBS="-L$(PKG_INSTALL_DIR) -lffi" \
			--with-libiconv=gnu \
			--prefix=$(PKG_INSTALL_DIR) \
			--enable-static=yes \
			--disable-FEATURE --disable-option-checking --disable-maintainer-mode --disable-dependency-tracking --disable-mem-pools --disable-largefile --disable-selinux --disable-fam --disable-xattr --disable-libelf --with-pcre=internal --with-threads=posix ; \
		echo "extern char **rpl_environ(void);" >> config.h; \
		echo "#define SOMAXCONN 4" >> config.h ; \
	)
	touch $@

$(BUILD) :
	cp stubs.c $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/gio/stubs.c; \
	cp stubs.h $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/gio/stubs.h; \
	cp libintl.h $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/glib/libintl.h; \
	cp -r include/arpa $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/gio/arpa; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) install; \
	); \
	cp -r $(PKG_INSTALL_DIR)/include/glib-2.0/* $(INCLUDE_INSTALL_DIR) ; \
	cp $(PKG_INSTALL_DIR)/lib/glib-2.0/include/* $(INCLUDE_INSTALL_DIR) ;
	
	touch $@

