PKG_NAME := libffi
PKG_VER  := 3.4.4

PKG_SOURCES := https://github.com/libffi/libffi/releases/download/v3.4.4/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5 := 0da1a5ed7786ac12dcbaf0d499d8a049

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		CC=$(EMBOX_GCC) ./configure \
				--host=$(AUTOCONF_TARGET_TRIPLET) \
				--enable-static=yes \
				--prefix=$(PKG_INSTALL_DIR) \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		cp $(AUTOCONF_TARGET_TRIPLET)/.libs/libffi.a $(PKG_INSTALL_DIR)/libffi.a; \
		cp $(AUTOCONF_TARGET_TRIPLET)/.libs/libffi.a $(PKG_INSTALL_DIR)/libffi.a; \
		\
		cp $(AUTOCONF_TARGET_TRIPLET)/include/ffi.h $(PKG_INSTALL_DIR)/ffi.h; \
		cp $(AUTOCONF_TARGET_TRIPLET)/include/ffitarget.h $(PKG_INSTALL_DIR)/ffitarget.h; \
		\
		cp $(AUTOCONF_TARGET_TRIPLET)/include/ffi.h $(INCLUDE_INSTALL_DIR)/ffi.h; \
		cp $(AUTOCONF_TARGET_TRIPLET)/include/ffitarget.h $(INCLUDE_INSTALL_DIR)/ffitarget.h; \
	)
	touch $@
