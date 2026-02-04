
PKG_NAME    := stress-ng
PKG_VER     := 0.20.00
PKG_SOURCES := https://github.com/ColinIanKing/$(PKG_NAME)/archive/refs/tags/V$(PKG_VER).tar.gz
PKG_MD5     := 0d1b99921cd69b4fa49a514dc5c2222c
PKG_PATCHES :=

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		STATIC=1 CC=$(EMBOX_CC) \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
	)
	touch $@

$(INSTALL) :
	$(CP) $(PKG_SOURCE_DIR)/stress-ng $(PKG_INSTALL_DIR)/stress-ng.o
	touch $@
