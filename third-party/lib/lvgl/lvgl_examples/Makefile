PKG_NAME := lvgl_demos
PKG_VER := v7.10.0
PKG_MD5 := 6d7ac30af3fb82c57008f6a49ea2497d

PKG_SOURCES := https://github.com/lvgl/lv_demos/archive/$(PKG_VER).tar.gz

include $(EXTBLD_LIB)

LVGL_CPP_FLAGS:=-DLV_CONF_PATH=$(CONF_DIR)/lvgl/lv_conf.h \
                -DLV_EX_CONF_PATH=$(CONF_DIR)/lvgl/lv_ex_conf.h \
                -DLV_LVGL_H_INCLUDE_SIMPLE

$(BUILD) :
	cp $(ROOT_DIR)/third-party/lib/lvgl/lvgl_examples/CMakeLists.txt $(PKG_SOURCE_DIR) \
	    && cd $(PKG_SOURCE_DIR) && (mkdir build/ || true) && cd build && \
	cmake \
	    -D CMAKE_C_FLAGS="$(LVGL_CPP_FLAGS)" \
	    -D CMAKE_C_COMPILER="$(EMBOX_GCC)" \
	    -D CMAKE_CXX_COMPILER="$(EMBOX_GXX)" \
	    -D CMAKE_C_COMPILER_WORKS=1 \
	    -D CMAKE_CXX_COMPILER_WORKS=1 \
	    .. && \
	make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/build/liblvglexamples.a $(PKG_INSTALL_DIR)
	touch $@
