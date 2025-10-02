
PKG_NAME    := waf
PKG_VER     := 35eadbb64e2052099a853b571e507c33032b392c
PKG_SOURCES := https://github.com/ArduPilot/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := dd89310bc38195677e4d77b70f1f84c7

$(CONFIGURE):
	touch $@

$(BUILD):
	touch $@

$(INSTALL):
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR); \
	touch $@
