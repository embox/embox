PKG_NAME := SEGGER_RTT
PKG_VER  := V672

PKG_SOURCES := http://localhost/$(PKG_NAME)_$(PKG_VER).tgz
PKG_MD5     := 94089b67d4c1ecdb13aa9e395153b88e

verify:
	@if [ ! -f $(DOWNLOAD_DIR)/$(PKG_NAME)-$(PKG_VER).tgz ]; then \
		echo ""; \
		echo $(DOWNLOAD_DIR)/$(PKG_NAME)-$(PKG_VER).tgz" not found\n"; \
		echo "RTT is under SEGGER License. Please download archive here:"; \
		echo "  https://www.segger.com/downloads/jlink/JLink_Linux_V672_x86_64.tgz"; \
		echo "Unzip and copy SEGGER_RTT_V672.tgz to SEGGER_RTT-V672.tgz in Embox download folder:"; \
		echo "  cp JLink_Linux_V672_x86_64/Samples/RTT/SEGGER_RTT_V672.tgz embox/download/SEGGER_RTT-V672.tgz"; \
		echo ""; \
		false; \
	fi

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/RTT && ( \
		$(EMBOX_GCC) -DCONFIG_RTT -o SEGGER_RTT.o SEGGER_RTT.c; \
		$(EMBOX_GCC) -DCONFIG_RTT -o SEGGER_RTT_printf.o SEGGER_RTT_printf.c; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR)/RTT && ( \
		cp SEGGER_RTT.h $(PKG_INSTALL_DIR); \
		cp SEGGER_RTT_Conf.h $(PKG_INSTALL_DIR); \
		cp SEGGER_RTT.o $(PKG_INSTALL_DIR); \
		cp SEGGER_RTT_printf.o $(PKG_INSTALL_DIR); \
	)
	touch $@
