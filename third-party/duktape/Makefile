
PKG_NAME := duktape
PKG_VER  := 2.5.0

PKG_SOURCES := https://duktape.org/duktape-2.5.0.tar.xz
PKG_MD5     := e55fe3830f0d469dc67205b380515af0

PKG_PATCHES := pkg_patch.txt

include $(EXTBLD_TARGETS)

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) -f Makefile.cmdline MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' CC=$(EMBOX_GCC); \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/duk $(PKG_INSTALL_DIR)/duk.o
	touch $@
