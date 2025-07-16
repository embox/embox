
PKG_NAME := SDL2
PKG_VER  := 2.0.8

PKG_SOURCES := https://www.libsdl.org/release/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 3800d705cef742c6a634f202c37f263f

PKG_PATCHES := sdl2.patch

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf && ( \
		./configure \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=$(PKG_INSTALL_DIR) \
			--enable-static \
			--enable-audio=no \
			--enable-video-directfb=yes \
			--enable-directfb-shared=no \
			--enable-video-vulkan=no \
			--enable-video-dummy=no \
			--enable-video-opengl=yes \
			--with-x=no \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' V=1; \
		cp build/.libs/*.a $(PKG_INSTALL_DIR)/ \
	)
	touch $@

$(INSTALL) :
	touch $@
