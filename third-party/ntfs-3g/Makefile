
PKG_NAME := ntfs-3g_ntfsprogs
PKG_VER  := 2013.1.13

PKG_SOURCES := http://tuxera.com/opensource/$(PKG_NAME)-$(PKG_VER).tgz \
               http://ftp.osuosl.org/pub/blfs/conglomeration/ntfs-3g/$(PKG_NAME)-$(PKG_VER).tgz

PKG_MD5     := 2d6fb47ddf62b51733227126fe9227fe

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

samba_cppflags = -include $(ROOT_DIR)/third-party/ntfs-3g/ntfs-3g_embox_compat.h \
	-I$(ROOT_DIR)/third-party/ntfs-3g/include \
	${EMBOX_IMPORTED_CPPFLAGS}

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		autoconf && aclocal && automake; \
		./configure --prefix=$(PKG_INSTALL_DIR) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="$(samba_cppflags)" \
			--enable-mount-helper=no \
			--disable-ldconfig \
			--disable-largefile \
			--enable-shared=no \
			--with-fuse=external \
		)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/libntfs-3g  && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/include $(PKG_INSTALL_DIR)/include/ntfs-3g
	cp -R $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/include/ntfs-3g $(PKG_INSTALL_DIR)/include
	cp $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/libntfs-3g/.libs/libntfs-3g.a $(PKG_INSTALL_DIR)/libntfs-3g/.libs/libntfs-3g.a
	touch $@
