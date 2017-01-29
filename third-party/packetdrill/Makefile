
PKG_NAME := packetdrill
PKG_VER  := d799c219acfa

PKG_SOURCES := https://code.google.com/p/packetdrill.git
PKG_MD5     := -
PKG_SOURCE_DIR := $(BUILD_DIR)/$(PKG_NAME)/gtests/net/$(PKG_NAME)

PKG_PATCHES := packetdrill.patch

include $(EXTBLD_LIB)


$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) -f Makefile.common \
			MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CPPFLAGS+="-I$(abspath $(CURDIR))/include -include $(abspath $(CURDIR))/packetdrill_stubs.h" \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/packetdrill $(PKG_INSTALL_DIR)/packetdrill.o
	touch $@
