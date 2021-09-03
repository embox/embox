PKG_NAME := libqsbr

PKG_SOURCES := http://download.embox.rocks/$(PKG_NAME).zip
PKG_MD5     := 403a68c02ed63a48cbccb80d1af9a76a

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
		$(EMBOX_GCC) -C ebr.c -o ebr.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C gc.c -o gc.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C qsbr.c -o qsbr.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C t_gc.c -o t_gc.o $(IMPORTED_CFLAGS) && \
		$(EMBOX_GCC) -C t_stress.c -o t_stress.o $(IMPORTED_CFLAGS) && \
		$(CROSS_COMPILE)ar rcs $(PKG_NAME).a *.o; \
	)
	touch $@


