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
	$(EMBOX_GCC) src/hello.c -o hello
	touch $@

$(INSTALL) :
	cp hello $(PKG_INSTALL_DIR)/hello.o
	touch $@
