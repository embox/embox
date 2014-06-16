
include $(EXTBLD_LIB)

$(BUILD) :
	cd $(ROOT_DIR)/third-party/phoneme/hello && ( \
		ant -Dpreverify.exec=$(BUILD_DIR)/../cldc/install/preverify \
			-Dclasses.zip=$(BUILD_DIR)/../cldc/install/cldc_classes.zip \
			-Dappbuild.dir=$(BUILD_DIR) \
	)
	touch $@
