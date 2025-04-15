
PKG_NAME := DA1468x_DA15xxx_SDK_1.0.14.1081_hotfix_001

PKG_SOURCES := https://support.dialog-semiconductor.com/system/files/restricted/DA1468x_DA15xxx_SDK_1.0.14.1081.zip

PKG_MD5     := cfd1fc53947c2c6350cd8dd6afdac983

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && ( \
		cp -f libble_stack_da14681_01.a ../../sdk/DA1468x_DA15xxx_SDK_1.0.14.1081/sdk/interfaces/ble_stack/DA14681-01-Release/libble_stack_da14681_01.a \
	)
	touch $@

$(BUILD) :
	cd $(EXTERNAL_BUILD_DIR)/third_party/bsp/dialog/da14681/sdk/DA1468x_DA15xxx_SDK_1.0.14.1081/ && ( \
		patch -p1 < $(PKG_SOURCE_DIR)/sdk.diff \
	)
	touch $@

$(INSTALL) :
	touch $@