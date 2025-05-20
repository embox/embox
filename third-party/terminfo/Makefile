
PKG_NAME := ncurses
PKG_VER  := 6.4

PKG_SOURCES := https://github.com/mirror/ncurses/archive/refs/tags/v6.4.tar.gz
PKG_MD5     := b7877c4c6cebed996158396d052a4b1f

$(BUILD) :
	$(MKDIR) $(MOD_BUILD_DIR)/terminfo
	tic -e $(call option_get,STRING,term) \
		-o $(MOD_BUILD_DIR)/terminfo $(PKG_SOURCE_DIR)/misc/terminfo.src &> /dev/null
	touch $@
