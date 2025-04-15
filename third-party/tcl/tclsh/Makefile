
PKG_NAME := tcl
PKG_VER  := 8.6.1

PKG_SOURCES := http://sourceforge.net/projects/tcl/files/Tcl/$(PKG_VER)/$(PKG_NAME)$(PKG_VER)-src.tar.gz \
	http://download.embox.rocks/$(PKG_NAME)$(PKG_VER)-src.tar.gz
PKG_MD5     := aae4b701ee527c6e4e1a6f9c7399882e

PKG_PATCHES := patch.txt

tcl_cppflags = -include $(ROOT_DIR)/third-party/tcl/tclsh/tcl_embox_compat.h

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cd $(BUILD_DIR)/$(PKG_NAME)$(PKG_VER)/unix && ( \
		./configure --prefix=$(PKG_INSTALL_DIR) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="$(tcl_cppflags)" \
			--disable-shared \
			--disable-load \
			--disable-threads \
			--disable-dll-unloading \
			--enable-symbols \
	)
	touch $@

$(BUILD) :
	cd $(BUILD_DIR)/$(PKG_NAME)$(PKG_VER)/unix  && ( \
		$(MAKE) tclsh MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cp $(BUILD_DIR)/$(PKG_NAME)$(PKG_VER)/unix/tclsh $(PKG_INSTALL_DIR)/tclsh.o
	cp $(BUILD_DIR)/$(PKG_NAME)$(PKG_VER)/unix/libtcl8.6.a $(PKG_INSTALL_DIR)/libtcl8.6.a
	cp $(BUILD_DIR)/$(PKG_NAME)$(PKG_VER)/unix/libtclstub8.6.a $(PKG_INSTALL_DIR)/libtclstub8.6.a
	cd $(BUILD_DIR)/$(PKG_NAME)$(PKG_VER)/unix && ( \
		$(MAKE) install-headers \
	)
	touch $@
