PKG_NAME := gmp
PKG_VER := 6.3.0

PKG_SOURCES := https://ftp.gnu.org/gnu/gmp/$(PKG_NAME)-$(PKG_VER).tar.xz

PKG_PATCHES := fix_types.patch

PKG_MD5     := 956dc04e864001a9c22429f761f2c283

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure --prefix=$(PKG_INSTALL_DIR)  \
			CC=$(EMBOX_GCC) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--enable-static \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
