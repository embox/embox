
include $(EXTBLD_TARGETS)

$(BUILD) :
	cd $(ROOT_DIR)/third-party/phoneme/hello && ( \
		ant -Dpreverify.exec=$(MOD_BUILD_DIR)/../cldc/install/preverify \
			-Dclasses.zip=$(MOD_BUILD_DIR)/../cldc/install/cldc_classes.zip \
			-Dappbuild.dir=$(MOD_BUILD_DIR) \
	)
	touch $@
