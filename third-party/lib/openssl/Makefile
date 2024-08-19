PKG_NAME := openssl
PKG_VER := 1.0.2u

PKG_SOURCES := https://openssl.org/source/old/1.0.2/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_PATCHES := patches/fix.patch

# 1.0.2u - cdc2638f789ecc2db2c91488265686c1
# 1.0.2m - 10e9e37f492094b9ef296f68f24a7666
PKG_MD5     := cdc2638f789ecc2db2c91488265686c1

include $(EXTBLD_LIB)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./Configure \
		linux-armv4 \
		--cross-compile-prefix=$(ROOT_DIR)/mk/extbld/arch-embox- \
		no-idea \
		no-mdc2 \
		no-rc5 \
		no-ssl2 \
		no-ssl3 \
		no-asm \
		386 \
		no-dso \
		no-shared \
		-Wno-error=array-bounds \
		-Wno-error=format-overflow \
		--prefix=$(PKG_INSTALL_DIR) \
		-DPEDANTIC \
		"$(EMBOX_CFLAGS)" \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) depend MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) build_libs MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install_sw MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
