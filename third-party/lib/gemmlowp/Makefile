PKG_NAME := gemmlowp
PKG_VER  := 16e8662c34917be0065110bfcd9cc27d30f52fdf

PKG_SOURCES := https://github.com/google/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_MD5 := 3380ee9c7d25122eab78343d4e831e4f


$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/src ;
	cp -r $(PKG_SOURCE_DIR)/* $(PKG_INSTALL_DIR)/src/ ;
	$(MKDIR) $(PKG_INSTALL_DIR)/include ;
	cp -r $(PKG_SOURCE_DIR)/fixedpoint $(PKG_INSTALL_DIR)/include/ ;
	cp -r $(PKG_SOURCE_DIR)/public/* $(PKG_INSTALL_DIR)/include/ ;
	touch $@
