PKG_NAME := efm
PKG_VER  := v1.0.0-alpha

PKG_SOURCES := https://github.com/t0bro/efm/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := 8805ada164ce0ad6bb80f44ca82b92cd

include $(EXTBLD_LIB)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/$(PKG_NAME) $(PKG_INSTALL_DIR)/$(PKG_NAME).o
	touch $@
