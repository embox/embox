PKG_NAME := toml-c
PKG_VER := 1.0
PKG_SOURCES :=https://github.com/arp242/toml-c/archive/refs/heads/master.zip

PKG_MD5     := bacca3462fbf95d3dce99e505a6a9e97

include $(EXTBLD_LIB)

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

