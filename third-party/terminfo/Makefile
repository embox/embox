
PKG_NAME := terminfo.src
PKG_VER  :=

PKG_SOURCES := https://invisible-island.net/datafiles/current/terminfo.src.gz
PKG_MD5     := b893841a8ca58f3fb9c9a52d4d23f4ba

include $(EXTBLD_LIB)

define option_get_string
$(shell echo OPTION_STRING_embox__init__setup_tty_devfs__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/embox/init/setup_tty_devfs.h -)
endef

$(BUILD) :
	mkdir -p $(BUILD_DIR)/terminfo
	tic -e $(call option_get_string,term_name) \
		-o $(BUILD_DIR)/terminfo $(BUILD_DIR)/$(PKG_NAME) &> /dev/null
	touch $@
