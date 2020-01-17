PKG_NAME := liblpm

PKG_SOURCES := http://download.embox.rocks/$(PKG_NAME).zip
PKG_MD5     := 587f02892c40bc474faefc6444df90de

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
		$(EMBOX_GCC) -C lpm.c -o lpm.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C t_lpm.c -o t_lpm.o $(IMPORTED_CFLAGS) && \
		$(CROSS_COMPILE)ar rcs thmap.a *.o; \
	)
	touch $@


