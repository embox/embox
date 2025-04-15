
include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	$(EMBOX_GCC) -o $(BUILD_DIR)/app app.c
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	touch $@
