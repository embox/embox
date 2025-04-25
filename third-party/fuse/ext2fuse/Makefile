
PKG_NAME := ext2fuse
PKG_VER  := 0.8.1

PKG_SOURCES := http://downloads.sourceforge.net/project/$(PKG_NAME)/$(PKG_NAME)/$(PKG_VER)/$(PKG_NAME)-src-$(PKG_VER).tar.gz \
	http://download.embox.rocks/$(PKG_NAME)-src-$(PKG_VER).tar.gz
PKG_MD5     := 8926c6eeb9ea17846466ca4bd7143489

PKG_PATCHES := patch.txt

EXT2FUSE_CPPFLAGS = -I$(ROOT_DIR)/third-party/fuse/ext2fuse/include \
    ${EMBOX_IMPORTED_CPPFLAGS}

$(CONFIGURE) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-src-$(PKG_VER) && autoconf && ( \
        ./configure --prefix=$(PKG_INSTALL_DIR) \
            --host=$(AUTOCONF_TARGET_TRIPLET) \
            --target=$(AUTOCONF_TARGET_TRIPLET) \
            CC=$(EMBOX_GCC) \
			CPPFLAGS="$(EXT2FUSE_CPPFLAGS)" \
			LDFLAGS="$(EMBOX_IMPORTED_LDFLAGS)" \
    )
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-src-$(PKG_VER)/src && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/src
	cp -R $(MOD_BUILD_DIR)/$(PKG_NAME)-src-$(PKG_VER)/src/* $(PKG_INSTALL_DIR)/src
	touch $@
