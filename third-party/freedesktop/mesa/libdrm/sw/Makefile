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
			--disable-dependency-tracking \
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

	touch $@

