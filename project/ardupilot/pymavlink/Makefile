
PKG_NAME    := pymavlink
PKG_VER     := 8ba67079211a4315681bc84a44c37b383448d664
PKG_SOURCES := https://github.com/ArduPilot/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := 18dfb27a20a46aa46e593b35a22c8ab3

$(CONFIGURE):
	touch $@

$(BUILD):
	touch $@

$(INSTALL):
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR) && \
	touch $@
