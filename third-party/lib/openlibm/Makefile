
PKG_NAME := openlibm
PKG_VER := ef29d06db1b57c415cbee51b90b492ae89812b5d

PKG_DOWNLOADS_SEPARATE := true

PKG_SOURCES := https://github.com/JuliaLang/openlibm/archive/$(PKG_VER).tar.gz
PKG_MD5 := 4bc22ffc211f959080173587e81742ec

PKG_PATCHES := openlibm_embox.patch

include $(EXTBLD_LIB)

OLIBM_ARCH ?=
ifneq ($(filter x86 usermode86,$(ARCH)),)
OLIBM_ARCH := i386
endif
ifeq ($(ARCH), arm)
OLIBM_ARCH := $(subst -march=,,$(filter -march=%,$(CFLAGS)))
ifeq ($(strip $(OLIBM_ARCH)),)
$(error OLIBM_ARCH -march is undefined)
endif
endif

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		make \
			USEGCC=1 \
			ARCH=$(OLIBM_ARCH) \
			OS=Embox \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="-include $(SRC_DIR)/include/sys/cdefs.h -D__ELF__" \
			SFLAGS="-D__ELF__" \
			libopenlibm.a; \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/libopenlibm.a $(PKG_INSTALL_DIR)/
	cp $(PKG_SOURCE_DIR)/include/openlibm*.h $(PKG_INSTALL_DIR)/
	touch $@
