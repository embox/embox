
PKG_NAME := libmodbus
PKG_VER  := 3.1.11

PKG_SOURCES := https://github.com/stephane/libmodbus/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := ff089fe3047d1c7b0ad2b01713b5b657

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./autogen.sh; \
		./configure \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--prefix=$(PKG_INSTALL_DIR) \
			CC=$(EMBOX_GCC) \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) install MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
