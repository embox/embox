PKG_NAME    := mavlink
PKG_VER     := bb87bc7390af7f21d9ad33a45c8be02997fecd24
PKG_SOURCES := https://github.com/ArduPilot/$(PKG_NAME)/archive/$(PKG_VER).zip

PKG_MD5     := 1e40dc5520360f06d420ef904db24790


$(CONFIGURE):
	$(CP) -r $(ROOT_DIR)/build/extbld/project/ardupilot/pymavlink/install/* $(PKG_SOURCE_DIR)/pymavlink/ && \
	touch $@

$(BUILD):
	touch $@

$(INSTALL):
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR) && \
	touch $@
