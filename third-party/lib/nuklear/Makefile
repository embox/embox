PKG_NAME := nuklear
PKG_VER := ef2dcd3b779647e0140bb78863cb8439774e277b

PKG_SOURCES := https://github.com/vurtun/nuklear/archive/$(PKG_VER).zip
PKG_MD5     := 318736d16bc0f7f1808aba365826d6c0

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/nuklear.h $(PKG_INSTALL_DIR)/
