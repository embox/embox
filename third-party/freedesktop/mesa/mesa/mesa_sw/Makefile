#https://mesa.freedesktop.org/archive/mesa-17.0.3.tar.xz

PKG_NAME := mesa
PKG_VER  := 18.2.5

PKG_SOURCES := https://mesa.freedesktop.org/archive/$(PKG_NAME)-$(PKG_VER).tar.xz

PKG_MD5     := b31a43ebb8b37fb704a9a75c90bbd71b

PKG_PATCHES := patch.txt

MESA_CPPFLAGS := -I$(SRC_DIR)/include -I$(SRC_DIR)/compat/cxx/include
MESA_CPPFLAGS += -I$(SRC_DIR)/compat/libc/include -I$(SRC_DIR)/compat/posix/include
MESA_CPPFLAGS += -I$(SRC_DIR)/arch/$(EMBOX_ARCH)/include 
MESA_CPPFLAGS += -I$(SRC_DIR)/compat/linux/include
MESA_CPPFLAGS += -I$(ROOT_DIR)/build/base/gen/include
MESA_CPPFLAGS += -I$(SRC_DIR)/../build/extbld/third_party/freedesktop/mesa/libdrm/libdrm-2.4.96/
MESA_CPPFLAGS += -DHAVE_FUNC_ATTRIBUTE_WEAK

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-static  --disable-shared \
			--enable-osmesa  --with-gallium-drivers=swrast \
			--disable-driglx-direct --disable-dri \
			--disable-egl \
			--disable-largefile --disable-valgrind \
			--disable-gallium-llvm --disable-gallium-tests \
			--disable-va \
			--disable-gbm \
			--disable-vdpau \
			--disable-glx --disable-glx-tls \
			--disable-asm \
			--disable-gles1 --disable-gles2 \
			--disable-dependency-tracking \
			ZLIB_LIBS="-O2" \
			PTHREADSTUBS_LIBS="-O2" \
			PKG_CONFIG=true \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
			CFLAGS='$(MESA_CPPFLAGS)' \
			PTHREAD_LIBS='-O2' \
			CPPFLAGS='-O2 $(MESA_CPPFLAGS)' \
			LIBDRM_CFLAGS='-I$(EXTERNAL_BUILD_DIR)/third_party/freedesktop/mesa/libdrm/libdrm-2.4.96/include/drm/'; \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' LIBDRM_CFLAGS='' V=1; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/src/mesa/.libs/libmesagallium.a $(PKG_INSTALL_DIR)/libmesagallium.a
	cp $(PKG_SOURCE_DIR)/src/mapi/shared-glapi/.libs/libglapi.a $(PKG_INSTALL_DIR)/libglapi.a
	cp $(PKG_SOURCE_DIR)/src/mesa/drivers/osmesa/.libs/libOSMesa.a $(PKG_INSTALL_DIR)/libOSMesa.a
	cp $(PKG_SOURCE_DIR)/src/gallium/auxiliary/.libs/libgallium.a $(PKG_INSTALL_DIR)/libgallium.a
	touch $@
