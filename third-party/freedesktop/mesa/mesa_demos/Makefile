#https://mesa.freedesktop.org/archive/demos/8.3.0/mesa-demos-8.3.0.tar.bz2
PKG_NAME := mesa-demos
PKG_VER  := 8.3.0

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.bz2


PKG_SOURCES := https://mesa.freedesktop.org/archive/demos/$(PKG_VER)/$(PKG_ARCHIVE_NAME)

PKG_MD5     := 628e75c23c17394f11a316c36f8e4164

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
			./configure \
			--disable-shared --enable-static \
			--disable-fast-install \
			--disable-egl --disable-gles1 --disable-gles2 --disable-vg \
			--enable-osmesa  \
			--disable-libdrm  --disable-x11 --disable-wayland --disable-gbm  \
			--disable-freetype2 --disable-rbug \
			PKG_CONFIG_PATH=$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/glu_osmesa/glu-9.0.0/: \
				$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa-17.0.3/src/mesa/: \
				$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa-17.0.3/src/mesa/drivers/osmesa/ \
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
