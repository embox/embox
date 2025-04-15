#https://mesa.freedesktop.org/archive/mesa-17.0.3.tar.xz

PKG_NAME := mesa
PKG_VER  := 18.2.5

PKG_SOURCES := https://mesa.freedesktop.org/archive/$(PKG_NAME)-$(PKG_VER).tar.xz

PKG_MD5     := b31a43ebb8b37fb704a9a75c90bbd71b

PKG_PATCHES := patch.txt

MESA_CPPFLAGS := -I$(SRC_DIR)/include -I$(SRC_DIR)/compat/cxx/include
MESA_CPPFLAGS += -I$(SRC_DIR)/compat/libc/include -I$(SRC_DIR)/compat/posix/include
MESA_CPPFLAGS += -I$(SRC_DIR)/arch/$(EMBOX_ARCH)/include
MESA_CPPFLAGS += -I$(SRC_DIR)/arch/$(EMBOX_ARCH)/include -I$(SRC_DIR)/../build/extbld/third_party/freedesktop/mesa/libdrm_etnaviv/libdrm-2.4.96/
MESA_CPPFLAGS += -I$(SRC_DIR)/../build/extbld/third_party/freedesktop/mesa/libdrm_etnaviv/libdrm-2.4.96/include/drm/
MESA_CPPFLAGS += -I$(SRC_DIR)/../build/extbld/third_party/freedesktop/mesa/libdrm_etnaviv/libdrm-2.4.96/etnaviv -DHAVE_FUNC_ATTRIBUTE_WEAK

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	export PKG_CONFIG_PATH=$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libdrm_etnaviv/libdrm-2.4.96:$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libdrm_etnaviv/libdrm-2.4.96/etnaviv;\
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-static  --disable-shared \
			--enable-gallium-osmesa \
			--with-gallium-drivers=etnaviv \
			--disable-gallium-llvm --disable-gallium-tests \
			--disable-driglx-direct --disable-dri \
			--disable-egl \
			--disable-largefile --disable-valgrind \
			--disable-va \
			--disable-gbm \
			--disable-vdpau \
			--disable-glx --disable-glx-tls \
			--disable-asm \
			--disable-dependency-tracking \
			--enable-gles1 --enable-gles2 \
			--disable-xvmc \
			ZLIB_LIBS="-O2" \
			PTHREADSTUBS_LIBS="-O2" \
			PKG_CONFIG=true \
			HAVE_GALLIUM_SOFTWARE_PIPE=yes \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
			CPPFLAGS='$(MESA_CPPFLAGS) -DGALLIUM_ETNAVIV -DHAVE_PTHREADS' \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mapi/shared-glapi/.libs/libglapi.a $(PKG_INSTALL_DIR)/libglapi.a
	cp $(PKG_SOURCE_DIR)/src/gallium/targets/osmesa/.libs/libOSMesa.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/src/gallium/drivers/etnaviv/.libs/libetnaviv.a $(PKG_INSTALL_DIR)
	cp $(PKG_SOURCE_DIR)/src/gallium/auxiliary/pipe-loader/.libs/libpipe_loader_static.a $(PKG_INSTALL_DIR)/libpipe_loader_static.a
	cp $(PKG_SOURCE_DIR)/src/gallium/winsys/sw/wrapper/.libs/libwsw.a $(PKG_INSTALL_DIR)
	cp $(PKG_SOURCE_DIR)/src/gallium/winsys/etnaviv/drm/.libs/libetnavivdrm.a $(PKG_INSTALL_DIR)
	touch $@
