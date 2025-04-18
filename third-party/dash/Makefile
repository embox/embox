
PKG_NAME := dash
PKG_VER  := 0.5.7

PKG_SOURCES := http://gondor.apana.org.au/~herbert/dash/files/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := f6cedb10ae7258adb5ab17a10ae80d51

PKG_PATCHES := dash.patch

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-DJOBS=0 -I$(ROOT_DIR)/third-party/dash/include ${EMBOX_IMPORTED_CPPFLAGS}" \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' CC=$(EMBOX_GCC) ;\
	)
	touch $@

$(INSTALL) :
	cd $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) install; \
		mv $(PKG_INSTALL_DIR)/usr/local/bin/dash $(PKG_INSTALL_DIR)/dash.o; \
	)
	touch $@

