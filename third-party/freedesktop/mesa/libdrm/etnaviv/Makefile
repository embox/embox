#https://dri.freedesktop.org/libdrm/libdrm-2.4.79.tar.bz2
PKG_NAME := libdrm
PKG_VER  := 2.4.79

PKG_ARCHIVE_NAME := $(PKG_NAME)-$(PKG_VER).tar.bz2


PKG_SOURCES := https://dri.freedesktop.org/libdrm/$(PKG_ARCHIVE_NAME)

PKG_MD5     := 84ba6e8e6c97d2684938eb949e6dfdf8

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-etnaviv-experimental-api \
			--enable-static \
			--disable-shared \
			--disable-tegra-experimental-api \
			--disable-vmwgfx \
			--disable-libkms \
			--enable-valgrind=no \
			--disable-radeon \
			--disable-amdgpu \
			--disable-nouveau \
			--disable-freedreno \
			--disable-vc4 \
			--enable-cairo-tests=no \
			CC=$(EMBOX_GCC) \
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

	cp $(PKG_SOURCE_DIR)/tests/etnaviv/etnaviv_2d_test.c $(PKG_INSTALL_DIR)/etnaviv_2d_test.c
	touch $@

