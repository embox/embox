
PKG_NAME := terminfo.src
PKG_VER  :=

PKG_SOURCES := https://invisible-island.net/datafiles/current/terminfo.src.gz
PKG_MD5     := b893841a8ca58f3fb9c9a52d4d23f4ba

include $(EXTBLD_LIB)

$(BUILD) :
	tic -e '$(foreach term,$(sort $(TERM)),$(term),)' \
		-o $(BUILD_DIR)/terminfo $(BUILD_DIR)/$(PKG_NAME) &> /dev/null
	touch $@
