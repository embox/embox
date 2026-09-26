
PKG_NAME    := DSDL
PKG_VER     := b4653c7abc3c47cb31b16efa24ea755232774756
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := bcdb6e4d7d4ebbd7481bdb9363901964

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR) && \
	touch $@
