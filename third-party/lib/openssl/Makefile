PKG_NAME := openssl
PKG_VER := 3.6.0
PKG_SOURCES := https://github.com/$(PKG_NAME)/$(PKG_NAME)/releases/download/$(PKG_NAME)-$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz

PKG_MD5     := 77ab78417082f22a2ce809898bd44da0

api := $(call option_get,STRING,api)
api := $(if $(api),--api=$(api))

export EMBOX_GCC
export EMBOX_GXX
export EMBOX_CFLAGS
export EMBOX_CXXFLAGS

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cp $(THIRDPARTY_DIR)/lib/openssl/embox.conf $(PKG_SOURCE_DIR)/Configurations
	cd $(PKG_SOURCE_DIR) && ( \
		./Configure embox \
		$(api) \
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

