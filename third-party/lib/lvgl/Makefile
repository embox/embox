
PKG_NAME    := lvgl
PKG_VER     := $(strip $(call option_get,STRING,lvgl_version))
PKG_SOURCES := https://github.com/lvgl/lvgl/archive/$(PKG_VER).tar.gz

#PKG_VER := v7.10.0
PKG_MD5_v7.10.0     := 94d3ab47a72dab33a7b7e2026f38ccab
PKG_PATCHES_v7.10.0 := patch-7.10.0.txt

#PKG_VER := v7.11.0
PKG_MD5_v7.11.0     := a863854d61b64536be30d61f9101b009
PKG_PATCHES_v7.11.0 := 

#PKG_VER := v8.1.0
PKG_MD5_v8.1.0     := 2e2ce43854ab77ade4f6af0d4b175b0b
PKG_PATCHES_v8.1.0 := patch-8.1.0.txt

PKG_PATCHES := $(PKG_PATCHES_$(PKG_VER))
PKG_MD5     := $(PKG_MD5_$(PKG_VER))

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
	    -D CMAKE_SYSTEM_NAME=Generic \
	    .. && \
	make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/bin
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/*.h $(PKG_INSTALL_DIR)/include
	cp -r $(PKG_SOURCE_DIR)/src $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/build/liblvgl.a $(PKG_INSTALL_DIR)/bin
	touch $@
