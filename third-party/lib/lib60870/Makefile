
PKG_NAME := lib60870
PKG_VER  := 2.3.1
PKG_SOURCES := https://github.com/mz-automation/$(PKG_NAME)/archive/refs/tags/v$(PKG_VER).tar.gz
PKG_MD5     := 567a1623dce330b8a582dbfc974085ed

PKG_PATCHES := Makefile.patch
PKG_PATCHES += socket_linux.patch
PKG_PATCHES += serial_port_linux.patch

export EMBOX_CFLAGS
export EMBOX_LDFLAGS
export EMBOX_GCC

export TARGET=EMBOX
ifeq ($(call option_get,BOOLEAN,iec61850), 1)
	export WITHOUT_COMMON=1
	export WITHOUT_HAL=1
endif

ifeq ($(call option_get,BOOLEAN,tls_enable), 1)
	export WITH_MBEDTLS=1
endif

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full

$(BUILD) :
	cp config/$(call option_get,STRING,config).h $(PKG_SOURCE_DIR)/lib60870-C/config/lib60870_config.h
	cd $(PKG_SOURCE_DIR)/lib60870-C && ( \
	$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	cd $(PKG_SOURCE_DIR)/lib60870-C  && ( \
	$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' INSTALL_PREFIX='$(PKG_INSTALL_DIR)' install; \
	)
	touch $@
