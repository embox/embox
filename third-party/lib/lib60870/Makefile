
PKG_NAME := lib60870
PKG_VER  := 2.3.1
PKG_SOURCES := https://github.com/mz-automation/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 567a1623dce330b8a582dbfc974085ed

PKG_PATCHES := Makefile.patch
PKG_PATCHES += socket_linux.patch
PKG_PATCHES += serial_port_linux.patch

include $(EXTBLD_LIB)

export EMBOX_CFLAGS
export EMBOX_LDFLAGS
export EMBOX_GCC

define option_get_boolean
$(shell echo OPTION_BOOLEAN_third_party__lib__lib60870__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/lib/lib60870.h -)
endef

export TARGET=EMBOX
ifeq ($(call option_get_boolean,iec61850), 1)
	export WITHOUT_COMMON=1
	export WITHOUT_HAL=1
endif

ifeq ($(call option_get_boolean,tls_enable), 1)
	export WITH_MBEDTLS=1
endif

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/lib60870-C && ( \
	$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
 
$(INSTALL) :
	cd $(PKG_SOURCE_DIR)/lib60870-C  && ( \
	$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' INSTALL_PREFIX='$(PKG_INSTALL_DIR)' install; \
	)
	touch $@

