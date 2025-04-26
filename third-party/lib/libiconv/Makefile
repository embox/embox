PKG_NAME := libiconv
PKG_VER  := 1.17

PKG_SOURCES := https://ftp.gnu.org/pub/gnu/libiconv/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5  := d718cd5a59438be666d1575855be72c3

PKG_PATCHES := patch.txt

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		CC=$(EMBOX_GCC) ./configure \
				--host=$(AUTOCONF_TARGET_TRIPLET) \
				--enable-static=yes \
				--prefix=$(PKG_INSTALL_DIR) \
	)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	export EMBOX_GCC_LINK=full; \
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		cp lib/.libs/libiconv.la $(PKG_INSTALL_DIR)/libiconv.la; \
		cp lib/.libs/libiconv.a $(PKG_INSTALL_DIR)/libiconv.a; \
		cp libcharset/lib/.libs/libcharset.la $(PKG_INSTALL_DIR)/libcharset.la; \
		cp libcharset/lib/.libs/libcharset.a $(PKG_INSTALL_DIR)/libcharset.a; \
		\
		cp srclib/libicrt.a $(PKG_INSTALL_DIR)/libicrt.a; \
		cp src/iconv.o $(PKG_INSTALL_DIR)/iconv.o; \
		\
		cp libcharset/include/localcharset.h.inst $(PKG_INSTALL_DIR)/localcharset.h; \
		cp libcharset/include/libcharset.h.inst $(PKG_INSTALL_DIR)/libcharset.h; \
		cp include/iconv.h.inst $(PKG_INSTALL_DIR)/iconv.h; \
		\
		cp $(PKG_INSTALL_DIR)/localcharset.h $(INCLUDE_INSTALL_DIR)/localcharset.h; \
		cp $(PKG_INSTALL_DIR)/libcharset.h $(INCLUDE_INSTALL_DIR)/libcharset.h; \
		cp $(PKG_INSTALL_DIR)/iconv.h $(INCLUDE_INSTALL_DIR)/iconv.h; \
		$(EMBOX_GCC) $(PKG_INSTALL_DIR)/iconv.o -o $(PKG_INSTALL_DIR)/iconv $(PKG_INSTALL_DIR)/libicrt.a $(PKG_INSTALL_DIR)/libiconv.a -Wl,-rpath -Wl,$(PKG_INSTALL_DIR) -Wl,-rpath -Wl,$(PKG_INSTALL_DIR); \
	)
	touch $@
