
include $(EXTBLD_DEFINES)

PKG_NAME := terminfo.src
PKG_VER  :=

PKG_SOURCES := https://invisible-mirror.net/archives/ncurses/terminfo.src.gz
PKG_MD5     := 053bb48fc744d108fb64120ddf0e51e3

include $(EXTBLD_TARGETS)

$(BUILD) :
	mkdir -p $(MOD_BUILD_DIR)/terminfo
	tic -e $(call option_get,STRING,term) \
		-o $(MOD_BUILD_DIR)/terminfo $(MOD_BUILD_DIR)/$(PKG_NAME) &> /dev/null
	touch $@
