PKG_NAME := cpuinfo
PKG_VER  := e414c0446436ed34151de3158d18f8ae32e55d03

PKG_SOURCES := https://github.com/pytorch/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5 := 2e010f64384bf5de4141fd513543b249

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/src
	cd $(PKG_SOURCE_DIR) && ( \
		cp -r * $(PKG_INSTALL_DIR)/src/ ; \
	)
	touch $@
