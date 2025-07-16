
PKG_NAME    := niiet_riscv_sdk
PKG_VER     :=
PKG_SOURCES := https://gitflic.ru/project/niiet/niiet_riscv_sdk/file/downloadAll?branch=v1.1&format=zip
PKG_EXT     := zip
PKG_MD5     := dfa35fabf1bd42b44f01764dddbd2fde
PKG_PATCHES := 

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/src
	$(MKDIR) $(PKG_INSTALL_DIR)/include
	$(CP) $(MOD_BUILD_DIR)/platform/plib015/src/* $(PKG_INSTALL_DIR)/src
	$(CP) $(MOD_BUILD_DIR)/platform/plib015/inc/* $(PKG_INSTALL_DIR)/src
	$(CP) $(MOD_BUILD_DIR)/platform/plib015/inc/* $(PKG_INSTALL_DIR)/include
	$(CP) $(MOD_BUILD_DIR)/platform/Device/K1921VG015/Include/K1921VG015.h $(PKG_INSTALL_DIR)/src
	$(CP) $(MOD_BUILD_DIR)/platform/Device/K1921VG015/Include/K1921VG015.h $(PKG_INSTALL_DIR)/include
	touch $@
