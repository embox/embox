
PKG_NAME    := can-utils
PKG_VER     := 2025.01
PKG_SOURCES := https://github.com/linux-can/can-utils/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 52059a7100e30ab3be6412f993607cc7
PKG_PATCHES :=

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(CP) -r $(CURDIR)/include $(PKG_INSTALL_DIR)
	$(CP) -r $(PKG_SOURCE_DIR)/include $(PKG_INSTALL_DIR)
	$(CP) $(PKG_SOURCE_DIR)/terminal.h $(PKG_INSTALL_DIR)
	$(CP) $(PKG_SOURCE_DIR)/cansend.c $(PKG_INSTALL_DIR)
	$(CP) $(PKG_SOURCE_DIR)/candump.c $(PKG_INSTALL_DIR)
	$(CP) $(PKG_SOURCE_DIR)/lib.h $(PKG_INSTALL_DIR)
	$(CP) $(PKG_SOURCE_DIR)/lib.c $(PKG_INSTALL_DIR)
	touch $@
