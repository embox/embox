
PKG_NAME := fuse
PKG_VER  := 2.9.4

PKG_SOURCES := https://github.com/libfuse/libfuse/releases/download/fuse_2_9_4/$(PKG_NAME)-$(PKG_VER).tar.gz \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := ecb712b5ffc6dffd54f4a405c9b372d8

#PKG_PATCHES := patch.txt

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp -R $(MOD_BUILD_DIR)/$(PKG_NAME)-$(PKG_VER)/include/* $(PKG_INSTALL_DIR)/include
	touch $@
