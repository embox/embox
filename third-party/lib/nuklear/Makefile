PKG_NAME := nuklear
PKG_VER := 9f0bca461b028c1f8b638beeba1859045ebe1ac3

PKG_SOURCES := https://github.com/Immediate-Mode-UI/Nuklear/archive/$(PKG_VER).zip
PKG_MD5     := 6600531a3c0bb6ce6f4e009b4f03c4c1

PKG_PATCHES := patch.txt \
	           rawfb.patch

include $(EXTBLD_TARGETS)

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/nuklear.h $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/demo/x11_rawfb/nuklear_rawfb.h $(PKG_INSTALL_DIR)/
