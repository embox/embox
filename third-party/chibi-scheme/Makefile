
PKG_NAME := chibi-scheme
PKG_VER  := 0.6.1

PKG_SOURCES := https://github.com/ashinn/$(PKG_NAME)/archive/$(PKG_VER).tar.gz
PKG_MD5     := 16b69c3497b46dfb65af8d98549b037b

PKG_PATCHES := chibi.patch

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) CC=$(EMBOX_GCC) SEXP_USE_DL=0 chibi-scheme-static; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/chibi-scheme-static $(PKG_INSTALL_DIR)/repl.o
	cp -r $(PKG_SOURCE_DIR)/lib $(PKG_INSTALL_DIR)
	touch $@
