#https://mesa.freedesktop.org/archive/demos/8.3.0/mesa-demos-8.3.0.tar.bz2
PKG_NAME := mesa-demos
PKG_VER  := 8.3.0

PKG_SOURCES := https://mesa.freedesktop.org/archive/demos/$(PKG_NAME)-$(PKG_VER).tar.bz2

PKG_MD5     := 628e75c23c17394f11a316c36f8e4164

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf && ( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--disable-shared --enable-static \
			--disable-fast-install \
			--disable-egl --disable-gles1 --disable-gles2 --disable-vg \
			--enable-osmesa  \
			--disable-libdrm  --disable-x11 --disable-wayland --disable-gbm  \
			--disable-freetype2 --disable-rbug \
			DEMO_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/glu-9.0.0/include -I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/mesa-17.0.3/include" \
			OSMESA_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/glu-9.0.0/include -I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/mesa-17.0.3/include" \
			OSMESA_LIBS="-L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/install -L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/install"\
			GL_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/glu-9.0.0/include -I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/mesa-17.0.3/include" \
			GL_LIBS="-L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/install -L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/install" \
			GLU_CFLAGS="-I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/glu-9.0.0/include -I$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/mesa-17.0.3/include" \
			GLU_LIBS="-L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/mesa_osmesa/install -L$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libglu_osmesa/install" \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) CXX=$(EMBOX_GXX) \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/osdemos/osdemo $(PKG_INSTALL_DIR)/mesa_demos_osdemo.o
	touch $@
