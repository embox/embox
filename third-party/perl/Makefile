
include $(EXTBLD_LIB)

define option_get_string
$(shell echo OPTION_STRING_third_party__cmd__perl__$(1) | gcc -P -E -include $(SRCGEN_DIR)/include/config/third_party/cmd/perl.h -)
endef

$(CONFIGURE) :
	touch $@

$(BUILD) :
	cd $(PKG_INSTALL_DIR) && ( \
		$(EXTERNAL_BUILD_DIR)/third_party/staticperl/build/install/staticperl.sh \
			mkperl $(addprefix -M,$(call option_get_string,perl_modules)) \
	)
	touch $@

$(INSTALL) :
	mv $(PKG_INSTALL_DIR)/perl $(PKG_INSTALL_DIR)/perl.o
	touch $@
