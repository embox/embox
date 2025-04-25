
include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	$(EMBOX_GCC) -o $(MOD_BUILD_DIR)/app app.c
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	touch $@
