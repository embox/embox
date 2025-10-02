
PKG_NAME    := dronecan_dsdlc
PKG_VER     := 43d8a9ed909e18f169c001a0f418edc72269b36b
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := b23b645b766499fd77bc75d22155b000

$(CONFIGURE):
	touch $@

$(BUILD):
	touch $@

$(INSTALL):
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR) && \
	touch $@
