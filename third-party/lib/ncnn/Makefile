PKG_NAME := ncnn
PKG_VER  := 20250916

PKG_SOURCES := https://github.com/Tencent/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := da74ad0af613a8dd4d9a4a02a234ecef

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	mkdir -p $(PKG_INSTALL_DIR)/src

	cp -r $(PKG_SOURCE_DIR)/src/* $(PKG_INSTALL_DIR)/src/

	rm -rf $(PKG_INSTALL_DIR)/src/layer/arm

	cp $(MOD_DIR)/platform_embox.h $(PKG_INSTALL_DIR)/src/platform.h
	cp $(MOD_DIR)/features_embox.h $(PKG_INSTALL_DIR)/src/features.h
	cp $(MOD_DIR)/layer_type_enum.h $(PKG_INSTALL_DIR)/src/layer_type_enum.h
	cp $(MOD_DIR)/layer_declaration.h $(PKG_INSTALL_DIR)/src/layer_declaration.h
	cp $(MOD_DIR)/layer_registry.h $(PKG_INSTALL_DIR)/src/layer_registry.h
	cp $(MOD_DIR)/layer_shader_registry.h $(PKG_INSTALL_DIR)/src/layer_shader_registry.h
	cp $(MOD_DIR)/layer_shader_spv_data.h $(PKG_INSTALL_DIR)/src/layer_shader_spv_data.h
	cp $(MOD_DIR)/ncnn_export.h $(PKG_INSTALL_DIR)/src/ncnn_export.h
	cp $(MOD_DIR)/layer_shader_type_enum.h $(PKG_INSTALL_DIR)/src/layer_shader_type_enum.h

	cp $(MOD_DIR)/cwchar $(PKG_INSTALL_DIR)/src/cwchar

	touch $@
