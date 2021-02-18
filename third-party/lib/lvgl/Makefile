PKG_NAME := lvgl
PKG_VER := v7.10.0

PKG_SOURCES := https://github.com/lvgl/lvgl/archive/$(PKG_VER).tar.gz

PKG_MD5 := 94d3ab47a72dab33a7b7e2026f38ccab

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

LVGL_CPP_FLAGS:=-DLV_CONF_PATH=$(CONF_DIR)/lvgl/lv_conf.h

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && (mkdir build/ || true) && cd build && \
	cmake \
	    -D LIBLVGL_STATIC=1 \
	    -D CMAKE_C_FLAGS="$(LVGL_CPP_FLAGS)" \
	    -D CMAKE_C_COMPILER="$(EMBOX_GCC)" \
	    -D CMAKE_CXX_COMPILER="$(EMBOX_GXX)" \
	    -D CMAKE_C_COMPILER_WORKS=1 \
	    -D CMAKE_CXX_COMPILER_WORKS=1 \
	    .. && \
	make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/build/liblvgl.a $(PKG_INSTALL_DIR)
	touch $@
