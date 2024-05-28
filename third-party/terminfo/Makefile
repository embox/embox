
PKG_NAME := terminfo.src
PKG_VER  :=

PKG_SOURCES := https://invisible-mirror.net/archives/ncurses/terminfo.src.gz
PKG_MD5     := 053bb48fc744d108fb64120ddf0e51e3

include $(EXTBLD_LIB)

define option_get_string
$(shell echo OPTION_STRING_third_party__terminfo__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/terminfo.h -)
endef

$(BUILD) :
	mkdir -p $(BUILD_DIR)/terminfo
	tic -e $(call option_get_string,term) \
		-o $(BUILD_DIR)/terminfo $(BUILD_DIR)/$(PKG_NAME) &> /dev/null
	touch $@
