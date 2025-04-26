
PKG_NAME := pcre
PKG_VER  := 8.36

PKG_SOURCES := http://netcologne.dl.sourceforge.net/project/$(PKG_NAME)/$(PKG_NAME)/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.bz2 \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.bz2
PKG_MD5     := b767bc9af0c20bc9c1fe403b0d41ad97

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=/ \
			--disable-cpp \
			--disable-shared\
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) DESTDIR=$(PKG_INSTALL_DIR) install; \
	)
	cp $(PKG_INSTALL_DIR)/lib/libpcre*.a $(PKG_INSTALL_DIR)

	touch $@
