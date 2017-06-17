#ftp://ftp.freedesktop.org/pub/mesa/glu/
PKG_NAME := glu
PKG_VER  := 9.0.0

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.bz2


PKG_SOURCES := ftp://ftp.freedesktop.org/pub/mesa/glu/$(PKG_ARCHIVE_NAME)

PKG_MD5     := be9249132ff49275461cf92039083030

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
			/configure \
			--disable-shared --enable-static \
			--disable-debug --enable-osmesa \
			PKG_CONFIG_PATH=$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa-17.0.3/src/mesa/ \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	touch $@
