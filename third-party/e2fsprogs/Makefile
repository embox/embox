
PKG_NAME := e2fsprogs
PKG_VER  := 1.47.2

PKG_SOURCES := https://github.com/tytso/e2fsprogs/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 4f28c1c2acdfd7c34b604d9e9f288e32

PKG_PATCHES := patch.diff

E2FSPROGS_CPPFLAGS = -include $(ROOT_DIR)/third-party/e2fsprogs/e2fsprogs_embox_compat.h \
	-I$(ROOT_DIR)/third-party/e2fsprogs/include

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --prefix=$(PKG_INSTALL_DIR) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--disable-tls \
			--disable-backtrace \
			--disable-elf-shlibs \
			--disable-bsd-shlibs \
			--disable-fuse2fs \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install; \
	)
	cp $(PKG_SOURCE_DIR)/lib/support/libsupport.a $(PKG_INSTALL_DIR)/lib
	touch $@
