
PKG_NAME := fuse
PKG_VER  := 2.9.4

PKG_SOURCES := http://sourceforge.net/projects/fuse/files/fuse-2.X/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := ecb712b5ffc6dffd54f4a405c9b372d8

PKG_PATCHES := patch.txt

FUSE_LINUX_CPPFLAGS = -I$(ROOT_DIR)/third-party/fuse/fuse_linux/include \
    ${EMBOX_IMPORTED_CPPFLAGS}

$(CONFIGURE) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && autoconf && ( \
        ./configure --prefix=$(PKG_INSTALL_DIR) \
            --host=$(AUTOCONF_TARGET_TRIPLET) \
            --target=$(AUTOCONF_TARGET_TRIPLET) \
            CC=$(EMBOX_GCC) \
			CPPFLAGS="$(FUSE_LINUX_CPPFLAGS)" \
			LDFLAGS="$(EMBOX_IMPORTED_LDFLAGS)" \
    )
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp -R $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/include/* $(PKG_INSTALL_DIR)/include
	touch $@
