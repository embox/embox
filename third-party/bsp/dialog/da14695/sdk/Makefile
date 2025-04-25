
PKG_NAME := SDK_10.0.10.118
PKG_MD5  := 17ba2eba5a02146b0f4f6c6375829514

PKG_SOURCES := https://support.dialog-semiconductor.com/system/files/restricted/$(PKG_NAME).zip

PKG_PATCHES := patch_$(PKG_NAME).txt \
	           freertos_patch_$(PKG_NAME).txt \
	           ble_freertos_patch_$(PKG_NAME).txt \
	           sys_usb_patch_$(PKG_NAME).txt \
			   pwr_patch_$(PKG_NAME).txt \
			   hw_dma_patch_$(PKG_NAME).txt \
			   sys_timer_patch_$(PKG_NAME).txt \
			   sys_clock_mgr_patch_$(PKG_NAME).txt \
			   ad_pmu_patch_$(PKG_NAME).txt \
			   snc_hw_sys_patch_$(PKG_NAME).txt \
			   add_at25ql641_patch_$(PKG_NAME).txt

verify:
	@if [ ! -f $(DOWNLOAD_DIR)/$(PKG_NAME).zip ]; then \
		echo ""; \
		echo $(DOWNLOAD_DIR)/$(PKG_NAME).zip" not found\n"; \
		echo "SDK is mostly under Dialog proprietary License. Please download archive here:"; \
		echo "  $(PKG_SOURCES)"; \
		echo "Copy downloaded archive to Embox download folder:"; \
		echo "  cp $(PKG_NAME).zip embox/download/$(PKG_NAME).zip"; \
		echo ""; \
		false; \
	fi

$(CONFIGURE) :
	cp qspi_at25ql641.h $(PKG_SOURCE_DIR)/sdk/bsp/memory/include/
	touch $@
