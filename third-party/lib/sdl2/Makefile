
PKG_NAME := SDL2
PKG_VER  := 2.0.8

PKG_SOURCES := https://www.libsdl.org/release/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 3800d705cef742c6a634f202c37f263f

include $(EXTBLD_LIB)

EMBOX_IMPORTED_CFLAGS   += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS   += $(filter -O2,$(EMBOX_CFLAGS))

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		CC=$(EMBOX_GCC) ./configure  --host=i386-unknown-netbsd \
			--enable-static \
			--enable-audio=no \
			--enable-video-directfb=yes \
			--enable-directfb-shared=no \
			--enable-video-vulkan=no \
			--enable-video-dummy=no \
			--enable-video-opengl=yes \
			--with-x=no; \
		patch -p0 < $(SRC_DIR)/../third-party/lib/sdl2/patch.txt;\
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' V=1 CFLAGS='$(EMBOX_IMPORTED_CFLAGS)'; \
		mkdir ../install; \
		cp build/.libs/*.a ../install/ \
	)
	touch $@
