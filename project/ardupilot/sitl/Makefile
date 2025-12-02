
PKG_NAME    := ardupilot
PKG_VER     := 4.6.2
PKG_SOURCES := https://github.com/ArduPilot/$(PKG_NAME)/archive/refs/tags/Copter-$(PKG_VER).tar.gz
PKG_PATCHES := sitl.patch
PKG_MD5     := 20c25637166e1314a203eb6b1d2066a8

ARDUPILOT_BOARD := $(call option_get,STRING,board)

$(CONFIGURE):
	export EMBOX_GCC_LINK=full; \
	$(CP) -r $(ROOT_DIR)/build/extbld/project/ardupilot/waf/install/* $(PKG_SOURCE_DIR)/modules/waf && \
	$(CP) -r $(ROOT_DIR)/build/extbld/project/ardupilot/mavlink/install/* $(PKG_SOURCE_DIR)/modules/mavlink && \
	$(CP) -r $(ROOT_DIR)/build/extbld/project/ardupilot/dsdl/install/* $(PKG_SOURCE_DIR)/modules/DroneCAN/DSDL && \
	$(CP) -r $(ROOT_DIR)/build/extbld/project/ardupilot/dronecan_dsdlc/install/* $(PKG_SOURCE_DIR)/modules/DroneCAN/dronecan_dsdlc && \
	$(CP) -r $(ROOT_DIR)/build/extbld/project/ardupilot/libcanard/install/* $(PKG_SOURCE_DIR)/modules/DroneCAN/libcanard && \
	cd $(PKG_SOURCE_DIR) && ( \
		./waf configure \
			--toolchain=$(TOOLCHAIN_DIR)/embox \
			--board $(ARDUPILOT_BOARD) \
			--debug \
			--static \
	)
	touch $@

$(BUILD):
	cd $(PKG_SOURCE_DIR) && ( \
		./waf copter \
	)
	touch $@

$(INSTALL):
	cd $(PKG_SOURCE_DIR)/build/$(ARDUPILOT_BOARD) && ( \
		$(EMBOX_GCC) -fno-exceptions -Wl,--wrap,malloc \
			bin/arducopter \
			-o $(PKG_INSTALL_DIR)/arducopter.o \
			-static lib/libArduCopter_libs.a \
	)
	touch $@
