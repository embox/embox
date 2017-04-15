#https://mesa.freedesktop.org/archive/mesa-17.0.3.tar.xz

PKG_NAME := mesa
PKG_VER  := 17.0.3

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.xz

PKG_SOURCES := https://mesa.freedesktop.org/archive/$(PKG_ARCHIVE_NAME)

PKG_MD5     := 003072c044b3fb5eca8be2037f15e4ec

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
		./configure --host=arm-none-eabi --target=arm-none-eabi \
			--enable-static  --disable-shared \
			--with-gallium-drivers=etnaviv --enable-dri=no --enable-gbm=no \
			--disable-egl \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="${EMBOX_IMPORTED_CPPFLAGS}" \
			LDFLAGS="$(EMBOX_IMPORTED_LDFLAGS)" \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE); CC=$(EMBOX_GCC) \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/.libs/libdrm.a $(PKG_INSTALL_DIR)/libdrm.a
	cp $(PKG_SOURCE_DIR)/etnaviv/.libs/libdrm_etnaviv.a $(PKG_INSTALL_DIR)/libdrm_etnaviv.a
	touch $@
