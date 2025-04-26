
PKG_NAME := zlib
PKG_VER  := 1.2.8

PKG_SOURCES := http://zlib.net/fossils/$(PKG_NAME)-$(PKG_VER).tar.gz \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5     := 44d667c142d7cda120332623eab69f40

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		CC=$(EMBOX_GCC) ./configure \
	)
	touch $@

$(BUILD) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/ && ( \
		cp minigzip $(PKG_INSTALL_DIR)/minigzip.o; \
		cp libz.a $(PKG_INSTALL_DIR); \
		cp zlib.h $(INCLUDE_INSTALL_DIR); \
		cp zconf.h $(INCLUDE_INSTALL_DIR); \
	)
	touch $@
