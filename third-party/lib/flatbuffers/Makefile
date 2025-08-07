PKG_NAME := flatbuffers
PKG_VER  := 23.5.26

PKG_SOURCES := https://github.com/google/$(PKG_NAME)/archive/v$(PKG_VER).zip
PKG_MD5 := e87e8acd8e2d53653387ad78720316e2

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR) && ( \
		cp -r include/ $(PKG_INSTALL_DIR); \
	)
	touch $@
