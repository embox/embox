
PKG_NAME := lib60870
PKG_VER  := 2.3.1
PKG_SOURCES := https://github.com/mz-automation/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 567a1623dce330b8a582dbfc974085ed

PKG_PATCHES := Makefile.patch
PKG_PATCHES += socket_linux.patch
PKG_PATCHES += serial_port_linux.patch

include $(EXTBLD_LIB)

export EMBOX_CFLAGS
export EMBOX_LDFLAGS
export EMBOX_GCC

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full

$(BUILD) :

	cd $(PKG_SOURCE_DIR)/lib60870-C && ( \
	$(MAKE) TARGET=EMBOX MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
 
$(INSTALL) :
	cd $(PKG_SOURCE_DIR)/lib60870-C  && ( \
	$(MAKE) TARGET=EMBOX INSTALL_PREFIX='$(PKG_INSTALL_DIR)' install; \
	)
	touch $@

