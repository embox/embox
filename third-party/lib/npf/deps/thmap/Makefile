PKG_NAME := thmap

PKG_SOURCES := http://download.embox.rocks/$(PKG_NAME).zip
PKG_MD5     := 6ec4f363e46a4b4ef27bd7eab2fad53e

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
		$(EMBOX_GCC) -C murmurhash.c -o murmurhash.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C t_stress.c -o t_stress.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C t_tgmap.c -o t_tgmap.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C thmap.c -o thmap.o $(IMPORTED_CFLAGS) && \
		$(CROSS_COMPILE)ar rcs thmap.a *.o; \
	)
	touch $@


