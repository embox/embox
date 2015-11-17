
PKG_NAME := fuse
PKG_VER  := 2.9.4

PKG_SOURCES := http://sourceforge.net/projects/fuse/files/fuse-2.X/$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := ecb712b5ffc6dffd54f4a405c9b372d8

#PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp -R $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/include/* $(PKG_INSTALL_DIR)/include
	touch $@
