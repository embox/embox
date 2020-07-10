
PKG_NAME := SDK_10.0.8.105

PKG_SOURCES := https://support.dialog-semiconductor.com/system/files/restricted/SDK_10.0.8.105.zip

PKG_MD5     := ff136c082317acb3fe161daf3e02e6bb

PKG_PATCHES := patch.txt \
	           ble_patch.txt \
	           sys_usb_patch.txt \
	           hw_usb_da1469x_patch.txt \
			   pwr_patch.txt \
			   hw_dma_patch.txt

verify:
	@if [ ! -f $(DOWNLOAD_DIR)/$(PKG_NAME).zip ]; then \
		echo ""; \
		echo $(DOWNLOAD_DIR)/$(PKG_NAME).zip" not found\n"; \
		echo "SDK is under SEGGER License. Please download archive here:"; \
		echo "  https://support.dialog-semiconductor.com/system/files/restricted/SDK_10.0.8.105.zip"; \
		echo "Copy downloaded archive to Embox download folder:"; \
		echo "  cp SDK_10.0.8.105.zip embox/download/SDK_10.0.8.105.zip"; \
		echo ""; \
		false; \
	fi

include $(EXTBLD_LIB)
