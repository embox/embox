PKG_NAME := toml-c
PKG_VER := c072e53645e0bb75cd4ab1901df8df861874ac45
PKG_SOURCES := https://github.com/arp242/$(PKG_NAME)/archive/$(PKG_VER).zip

PKG_MD5     := 2584710bf290c778321a34b716249803

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) libtoml.a CC=$(EMBOX_GCC); \
	)
	touch $@

$(INSTALL) :
	$(MV) $(PKG_SOURCE_DIR)/libtoml.a $(PKG_INSTALL_DIR)/lib
	$(MV) $(PKG_SOURCE_DIR)/toml.h $(PKG_INSTALL_DIR)
	touch $@

