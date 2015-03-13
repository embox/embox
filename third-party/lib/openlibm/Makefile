
PKG_NAME := openlibm
PKG_VER := ef29d06db1b57c415cbee51b90b492ae89812b5d

PKG_DOWNLOADS_SEPARATE := true

PKG_SOURCES := https://github.com/JuliaLang/$(PKG_NAME)/archive/$(PKG_VER).zip
PKG_PATCHES := openlibm_embox.patch

PKG_MD5 := a8d4531e2d35384d244977fce009dbe2

include $(EXTBLD_LIB)

CPPFLAGS := -include $(SRC_DIR)/include/sys/cdefs.h -D__ELF__
SFLAGS := -D__ELF__

ifeq ($(ARCH),x86)
OLIBM_ARCH := ARCH=i386
endif
ifeq ($(ARCH), arm)
ARM_ARCH := $(subst -march=,,$(filter -march=%,$(CFLAGS)))
ifeq ($(strip $(ARM_ARCH)),)
$(error ARM_ARCH -march is undefined)
endif
OLIBM_ARCH := ARCH=$(ARM_ARCH)
endif

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make CC=$(EMBOX_GCC) CPPFLAGS="$(CPPFLAGS)" SFLAGS="$(SFLAGS)" \
			USEGCC=1 $(OLIBM_ARCH) \
			libopenlibm.a; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/libopenlibm.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/include/openlibm*.h $(PKG_INSTALL_DIR)/
	touch $@
