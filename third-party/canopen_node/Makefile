
PKG_NAME    := CANopenNode
PKG_VER     := 5a97d2d20962512823015bc12452d90c423c9f73
PKG_SOURCES := https://github.com/CANopenNode/CANopenNode/archive/$(PKG_VER).zip
PKG_MD5     := a6495685ce15d9a77524b9863fa0fd8d

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(PKG_INSTALL_DIR)/include
	rsync -a --prune-empty-dirs --include="*/" --include="*.c" --exclude="*" \
		$(PKG_SOURCE_DIR)/ $(PKG_INSTALL_DIR)
	rsync -a --prune-empty-dirs --include="*/" --include="*.h" --exclude="*" \
		$(PKG_SOURCE_DIR)/ $(PKG_INSTALL_DIR)/include
	touch $@
