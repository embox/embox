include $(EXTBLD_LIB)

$(DOWNLOAD):
	touch $@

$(DOWNLOAD_CHECK) : 
	touch $@

$(EXTRACT) : 
	touch $@

$(PATCH) : 
	touch $@

$(CONFIGURE) :
	touch $@

$(BUILD) :
	$(EMBOX_GCC) src/lkl-test.c -I$(PKG_INSTALL_DIR)/../../lkl/install/include -o lkltest
	touch $@

$(INSTALL) :
	cp lkltest $(PKG_INSTALL_DIR)/lkltest.o
	touch $@
