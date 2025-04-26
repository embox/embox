
PKG_NAME := tinypy
PKG_VER  := 1.1

PKG_SOURCES := https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/tinypy/tinypy-1.1.tar.gz
PKG_MD5     := 1091300b72fcc1f75cafb91bea92772e

PKG_PATCHES := setup.patch

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && CC=$(EMBOX_GCC) python2 setup.py linux
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/build/tinypy $(PKG_INSTALL_DIR)/tinypy.o
	touch $@
