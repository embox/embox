PKG_NAME := lvgl_examples
PKG_VER := v7.10.0

PKG_SOURCES := https://github.com/lvgl/lv_examples/archive/$(PKG_VER).tar.gz

PKG_MD5 := 1187435bc003dc7d4f58df354b0212f8

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
	make
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/build/liblvglexamples.a $(PKG_INSTALL_DIR)
	touch $@
