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
	mkdir -p $(PKG_INSTALL_DIR)/include/ncnn

	cp -r $(PKG_SOURCE_DIR)/src/* $(PKG_INSTALL_DIR)/src/

	rm -rf $(PKG_INSTALL_DIR)/src/layer/arm

	cp $(PKG_INSTALL_DIR)/src/*.h $(PKG_INSTALL_DIR)/include/ncnn/ || true

	cp $(MOD_DIR)/platform_embox.h $(PKG_INSTALL_DIR)/src/platform.h
	cp $(MOD_DIR)/features_embox.h $(PKG_INSTALL_DIR)/src/features.h

	cp $(MOD_DIR)/generated/layer_type_enum.h $(PKG_INSTALL_DIR)/src/layer_type_enum.h
	cp $(MOD_DIR)/generated/layer_declaration.h $(PKG_INSTALL_DIR)/src/layer_declaration.h
	cp $(MOD_DIR)/generated/layer_registry.h $(PKG_INSTALL_DIR)/src/layer_registry.h
	cp $(MOD_DIR)/generated/layer_shader_registry.h $(PKG_INSTALL_DIR)/src/layer_shader_registry.h
	cp $(MOD_DIR)/generated/layer_shader_spv_data.h $(PKG_INSTALL_DIR)/src/layer_shader_spv_data.h
	cp $(MOD_DIR)/generated/ncnn_export.h $(PKG_INSTALL_DIR)/src/ncnn_export.h
	cp $(MOD_DIR)/generated/layer_shader_type_enum.h $(PKG_INSTALL_DIR)/src/layer_shader_type_enum.h

	cp $(MOD_DIR)/platform_embox.h $(PKG_INSTALL_DIR)/include/ncnn/platform.h
	cp $(MOD_DIR)/features_embox.h $(PKG_INSTALL_DIR)/include/ncnn/features.h
	cp $(MOD_DIR)/generated/ncnn_export.h $(PKG_INSTALL_DIR)/include/ncnn/ncnn_export.h

	cp $(MOD_DIR)/cwchar $(PKG_INSTALL_DIR)/src/cwchar

	touch $@