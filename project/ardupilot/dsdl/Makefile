
PKG_NAME    := DSDL
PKG_VER     := 0856687a5acb42b55d29bbad8f4fc9e19b4989fc
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := ee50c611fc84905d9eb1a3d1eade4dbe

$(CONFIGURE):
	touch $@

$(BUILD):
	touch $@

$(INSTALL):
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR) && \
	touch $@
