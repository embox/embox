#https://mesa.freedesktop.org/archive/mesa-17.0.3.tar.xz

PKG_NAME := mesa
PKG_VER  := 17.0.3

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.xz

PKG_SOURCES := https://mesa.freedesktop.org/archive/$(PKG_ARCHIVE_NAME)

PKG_MD5     := 003072c044b3fb5eca8be2037f15e4ec

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)


MESA_CPPFLAGS := -I$(SRC_DIR)/include -I$(SRC_DIR)/compat/cxx/include 
MESA_CPPFLAGS += -I$(SRC_DIR)/compat/libc/include -I$(SRC_DIR)/compat/posix/include
MESA_CPPFLAGS += $(SRC_DIR))/arch/$(ARCH)/include

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	export PKG_CONFIG_PATH=$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libdrm_imx6/libdrm-2.4.79:$(ROOT_DIR)/build/extbld/third_party/freedesktop/mesa/libdrm_imx6/libdrm-2.4.79/etnaviv;\
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
		./configure --host=arm-none-eabi --target=arm-none-eabi \
			--enable-static  --disable-shared \
			--with-gallium-drivers=etnaviv --enable-dri=no --enable-gbm=no \
			--disable-egl \
			--disable-largefile --enable-gallium-llvm=no --enable-valgrind=no \
			CC=$(EMBOX_GCC) \
			CXX=$(EMBOX_GXX) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE); \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/.libs/libdrm.a $(PKG_INSTALL_DIR)/libdrm.a
	cp $(PKG_SOURCE_DIR)/etnaviv/.libs/libdrm_etnaviv.a $(PKG_INSTALL_DIR)/libdrm_etnaviv.a
	touch $@
