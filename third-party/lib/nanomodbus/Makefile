PKG_NAME := nanomodbus
PKG_VER := 1.20.1

PKG_SOURCES := https://github.com/debevv/nanoMODBUS/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5 := 0a9941241e3ff277dd02f8d25cb42cd7

RANLIB ?= $(ROOT_DIR)/mk/extbld/arch-embox-ranlib

$(BUILD):
	cd $(PKG_SOURCE_DIR) && (mkdir build/ || true) && cd build && \
	cmake \
		-D CMAKE_SYSTEM_NAME=Generic \
		-D CMAKE_C_COMPILER="$(EMBOX_GCC)" \
		-D CMAKE_RANLIB="$(RANLIB)" \
		-D CMAKE_C_COMPILER_WORKS=1 \
		.. && \
	$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/lib
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/*.h $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/build/libnanomodbus.a $(PKG_INSTALL_DIR)/lib/
	$(RANLIB) $(PKG_INSTALL_DIR)/lib/libnanomodbus.a
	touch $@

