
PKG_NAME := chibi-scheme
PKG_VER  := 0.6.1

PKG_SOURCES := https://chibi-scheme.googlecode.com/files/$(PKG_NAME)-$(PKG_VER).tgz
PKG_MD5     := 02247ed0932fe3824e7c8c69c63e37c3

PKG_PATCHES := chibi.patch

include $(EXTBLD_LIB)

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make CC=$(EMBOX_GCC) SEXP_USE_DL=0 chibi-scheme-static; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/chibi-scheme-static $(PKG_INSTALL_DIR)/repl.o
	cp -r $(PKG_SOURCE_DIR)/lib $(PKG_INSTALL_DIR)
	touch $@
