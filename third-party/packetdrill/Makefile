
PKG_NAME := packetdrill
PKG_VER  := d799c219acfa

PKG_SOURCES := https://storage.googleapis.com/google-code-archive-source/v2/code.google.com/packetdrill/source-archive.zip
PKG_MD5     := 1c203d1474022b0aad2364f3ab92494e

PKG_PATCHES := packetdrill.patch

PACKETDRILL_SOURCE_DIR := $(MOD_BUILD_DIR)/$(PKG_NAME)/gtests/net/$(PKG_NAME)

CFLAGS_NO_PACKED =

GCCVERSIONGTEQ9 := $(shell expr `$(EMBOX_GCC) -dumpversion | cut -f1 -d.` \>= 9)

ifeq "$(GCCVERSIONGTEQ9)" "1"
    CFLAGS_NO_PACKED = -Wno-address-of-packed-member
endif

$(BUILD) :
	cd $(PACKETDRILL_SOURCE_DIR) && ( \
		$(MAKE) -f Makefile.common \
			MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
			CC=$(EMBOX_GCC) \
			CFLAGS+=$(CFLAGS_NO_PACKED) \
			CPPFLAGS+="-I$(abspath $(CURDIR))/include -include $(abspath $(CURDIR))/packetdrill_stubs.h" \
	)
	touch $@

$(INSTALL) :
	cp $(PACKETDRILL_SOURCE_DIR)/packetdrill $(PKG_INSTALL_DIR)/packetdrill.o
	touch $@
