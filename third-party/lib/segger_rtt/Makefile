PKG_NAME := SEGGER_RTT
PKG_VER  := V672

# TODO. Not uploaded yet!
# https://www.segger.com/downloads/jlink/JLink_Linux_x86_64.tgz
# It's located at Samples/RTT/SEGGER_RTT_V672.tgz
PKG_SOURCES := http://download.embox.rocks/$(PKG_NAME)_$(PKG_VER).tgz
PKG_MD5     := 94089b67d4c1ecdb13aa9e395153b88e

include $(EXTBLD_LIB)

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
