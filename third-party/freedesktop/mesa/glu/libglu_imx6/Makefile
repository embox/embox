#ftp://ftp.freedesktop.org/pub/mesa/glu/
PKG_NAME := glu
PKG_VER  := 9.0.0

PKG_SOURCES := ftp://ftp.freedesktop.org/pub/mesa/glu/$(PKG_NAME)-$(PKG_VER).tar.bz2

PKG_MD5     := be9249132ff49275461cf92039083030

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoreconf --force --install && automake --add-missing && ( \
			./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--disable-shared --enable-static \
			--disable-debug --enable-osmesa \
			GL_LIBS="-L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_etnaviv/install" \
			GL_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_etnaviv/mesa-18.2.5/include" \
			OSMESA_LIBS="-L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_etnaviv/install" \
			OSMESA_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_etnaviv/mesa-18.2.5/include" \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX); \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/.libs/libGLU.a $(PKG_INSTALL_DIR)/libGLU.a
	touch $@
