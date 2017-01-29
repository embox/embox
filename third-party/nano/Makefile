
PKG_NAME := nano
PKG_VER  := 2.2.6

PKG_SOURCES := http://www.nano-editor.org/dist/v2.2/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 03233ae480689a008eb98feb1b599807

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-tiny \
			--disable-shared \
			--disable-static \
			--disable-largefile \
			--disable-rpath \
			--disable-nls \
			--disable-extra \
			--disable-browser \
			--disable-help \
			--disable-justify \
			--disable-mouse \
			--disable-operatingdir \
			--disable-speller \
			--disable-tabcomp \
			--disable-wrapping \
			--disable-wrapping-as-root \
			--disable-color \
			--disable-multibuffer \
			--disable-nanorc \
			--disable-glibtest \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/nano $(PKG_INSTALL_DIR)/nano.o
	touch $@
