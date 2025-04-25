
define option_get_string
$(shell echo OPTION_STRING_third_party__lib__lvgl__$(1) | \
gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/lib/lvgl.h -)
endef

PKG_NAME := lvgl_demos
PKG_VER  := $(call option_get_string,lvgl_version)

#PKG_VER := v7.10.0
PKG_MD5_v7.10.0 := 6d7ac30af3fb82c57008f6a49ea2497d

#PKG_VER := v7.11.0
PKG_MD5_v7.11.0 := 598e406a7fb295cbf93e887b373fa003

#PKG_VER := v8.1.0
PKG_MD5_v8.1.0 := 4c4d7dc1e18feddfac77193006764bda

PKG_MD5 := $(PKG_MD5_$(PKG_VER))

PKG_SOURCES := https://github.com/lvgl/lv_demos/archive/$(PKG_VER).tar.gz

LVGL_CPP_FLAGS:=-DLV_CONF_PATH=$(CONF_DIR)/lvgl/lv_conf.h \
                -DLV_EX_CONF_PATH=$(CONF_DIR)/lvgl/lv_ex_conf.h \
                -DLV_DEMO_CONF_PATH=$(CONF_DIR)/lvgl/lv_demo_conf.h \
                -DLV_LVGL_H_INCLUDE_SIMPLE \
                -DLV_DEMO_CONF_INCLUDE_SIMPLE

$(BUILD) :
	cp $(ROOT_DIR)/third-party/lib/lvgl/lv_demos/CMakeLists.txt $(PKG_SOURCE_DIR) \
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
	mkdir -p $(PKG_INSTALL_DIR)/bin
	mkdir -p $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/*.h $(PKG_INSTALL_DIR)/include
	cp -r $(PKG_SOURCE_DIR)/src $(PKG_INSTALL_DIR)/include
	cp $(PKG_SOURCE_DIR)/build/liblvglexamples.a $(PKG_INSTALL_DIR)/bin
	touch $@
