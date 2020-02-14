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
	if [ ! -d linux ]; then \
		git clone https://github.com/lkl/linux; \
	fi; \
	cd linux; \
	git checkout 7750a5aa74f5867336949371f0e18353704d432f; \
	git apply ../../../lkl-i386-support.patch; \
	git apply ../../../lkl-embox-support.patch; \
	cd tools/lkl && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS) V=1' CC='$(EMBOX_GCC)'; \
	)
	touch $@

$(INSTALL) :
	cd linux/tools/lkl && ( \
		cp -r include $(PKG_INSTALL_DIR); \
		cp liblkl.a $(PKG_INSTALL_DIR); \
	)
	touch $@
