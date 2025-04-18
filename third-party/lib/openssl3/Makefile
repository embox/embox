PKG_NAME := openssl
PKG_VER := 3.3.1
PKG_SOURCES := https://github.com/$(PKG_NAME)/$(PKG_NAME)/releases/download/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_PATCHES := embox.patch

PKG_MD5     := 8a4342b399c18f870ca6186299195984

include $(EXTBLD_TARGETS)

export EMBOX_GCC
export EMBOX_GXX
export EMBOX_CFLAGS
export EMBOX_CXXFLAGS

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cp $(THIRDPARTY_DIR)/lib/openssl3/embox.conf $(PKG_SOURCE_DIR)/Configurations
	cd $(PKG_SOURCE_DIR) && ( \
		./Configure embox \
		--prefix=$(PKG_INSTALL_DIR) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) build_sw MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install_sw MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

