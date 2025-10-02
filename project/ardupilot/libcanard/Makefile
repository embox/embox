
PKG_NAME    := libcanard
PKG_VER     := 9b05a4d8eebf82684053d07e6d04fe2b0c922a6e
PKG_SOURCES := https://github.com/dronecan/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5     := 9bd8cee80e20e3b0d28a6b6f692b70d3

$(CONFIGURE):
	touch $@

$(BUILD):
	touch $@

$(INSTALL):
	$(CP) -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR) && \
	touch $@
