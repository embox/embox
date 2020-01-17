PKG_NAME := libnv

PKG_SOURCES := http://download.embox.rocks/$(PKG_NAME).zip
PKG_MD5     := a7aca1056d848cc3a5d95e0cd3be9201

include $(EXTBLD_LIB)

#prefix=$(PKG_INSTALL_DIR) 
CC=$(EMBOX_GCC)

IMPORTED_CFLAGS=$(CFLAGS) $(EMBOX_CPPFLAGS)

VARS_OLD := $(.VARIABLES)
$(foreach v,                                        \
  $(filter-out $(VARS_OLD) VARS_OLD,$(.VARIABLES)), \
  $(info $(v) = $($(v))))

$(BUILD) :
	cd $(PKG_SOURCE_DIR)/src/ && ( \
		CC=$(EMBOX_GCC) \
		CFLAGS="$(IMPORTED_CFLAGS)" \
		LIBDIR="$(PKG_INSTALL_DIR)" \
		$(MAKE) lib MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@
