
include $(EXTBLD_DEFINES)

PERL_MODULES := $(call option_get,STRING,perl_modules)

include $(EXTBLD_TARGETS)

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_INSTALL_DIR) && ( \
		$(EXTERNAL_BUILD_DIR)/third_party/staticperl/build/install/staticperl.sh \
			mkperl $(addprefix -M,$(PERL_MODULES)) \
	)
	touch $@

$(INSTALL) :
	mv $(PKG_INSTALL_DIR)/perl $(PKG_INSTALL_DIR)/perl.o
	touch $@
