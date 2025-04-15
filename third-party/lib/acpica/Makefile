PKG_NAME := acpica
PKG_VER  := R06_28_23

PKG_SOURCES := https://github.com/acpica/$(PKG_NAME)/archive/refs/tags/$(PKG_VER).tar.gz
PKG_MD5     := ca8425f76b1b42cb4956e81025c1df6d

PKG_PATCHES := acenv.h.patch

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	touch $@