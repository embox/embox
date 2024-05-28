
PKG_NAME := nano
PKG_VER  := 7.2

PKG_SOURCES := http://www.nano-editor.org/dist/v7/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 2fc38daa9075b7e29f69085c63a5aa8a

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-tiny \
			--disable-nls \
			--disable-utf8 \
			--disable-rpath \
			--disable-largefile \
			--disable-multibuffer \
			--without-included-regex \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/src && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			nano_LDADD='' nano_DEPENDENCIES='' LIBS='' \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/nano $(PKG_INSTALL_DIR)/nano.o
	touch $@
