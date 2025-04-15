PKG_NAME := libqsbr
PKG_VER := e73407cb5dba097e59036e9a4ad8cfe8f5fe50dd

#PKG_SOURCES := git@github.com:rmind/libqsbr.git
PKG_SOURCES := https://github.com/rmind/libqsbr/archive/$(PKG_VER).tar.gz

PKG_MD5     := bc173ff2caaf3d27b42befe0147369fa

PKG_PATCHES := 32bit_counter_patch.txt

include $(EXTBLD_TARGETS)

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/qsbr
	cp $(PKG_SOURCE_DIR)/src/ebr.h $(PKG_INSTALL_DIR)/qsbr
	cp $(PKG_SOURCE_DIR)/src/qsbr.h $(PKG_INSTALL_DIR)/qsbr