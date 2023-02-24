
PKG_NAME := mosquitto
PKG_VER  := 2.0.15

PKG_SOURCES := https://github.com/eclipse/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 1247e425545affe185278a5314e7bbfe

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
	cp $(PKG_SOURCE_DIR)/src/mosquitto $(PKG_INSTALL_DIR)/mosquitto.o
	touch $@
