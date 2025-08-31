
PKG_NAME := en.AudioPDM-MW_V3-1-0
PKG_MD5  := 028887f8564d1812bf92a06452b1a639

PKG_SOURCES := https://localhost/$(PKG_NAME).zip

DOWNLOADED_LINK := https://my.st.com/content/my_st_com/en/products/licensed-software/stm32-licensed-software/audiopdm-mw.license=1621431532619.product=AudioPDM-MW.version=3.1.0.html

$(CONFIGURE) :
	@if [ ! -f $(DOWNLOAD_DIR)/$(PKG_NAME).zip ]; then \
		echo ""; \
		echo $(DOWNLOAD_DIR)/$(PKG_NAME).zip" not found\n"; \
		echo "STM32Cube middleware for audio PDM to PCM conversion . Please download archive here:"; \
		echo "  $(DOWNLOADED_LINK)"; \
		echo "Copy downloaded archive to Embox download folder:"; \
		echo "  cp $(PKG_NAME).zip embox/download/$(PKG_NAME).zip"; \
		echo ""; \
		false; \
	fi
