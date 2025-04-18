#https://dri.freedesktop.org/libdrm/libdrm-2.4.79.tar.bz2
PKG_NAME := libdrm
PKG_VER  := 2.4.96

PKG_SOURCES := https://dri.freedesktop.org/libdrm/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5     := dd0a89154ee3d60c37ee5b8099979875

PKG_PATCHES := patch.txt

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && aclocal && autoconf &&( \
		./configure --host=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-etnaviv-experimental-api \
			--enable-static \
			--disable-dependency-tracking \
			--disable-shared \
			--disable-tegra-experimental-api \
			--disable-vmwgfx \
			--disable-libkms \
			--enable-valgrind=no \
			--disable-dependency-tracking \
			--disable-dri \
			--disable-dri \
			--disable-va \
			--disable-radeon \
			--disable-amdgpu \
			--disable-nouveau \
			--disable-freedreno \
			--disable-vc4 \
			--enable-cairo-tests=no \
			CFLAGS="-I$(PKG_SOURCE_DIR)/etnaviv/" \
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

