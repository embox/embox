
.PHONY : OPTION_CHECK

OPTION_CHECK_H := $(SRCGEN_DIR)/option_check.h
OPTION_CHECK_MK := $(CACHE_DIR)/option_check.mk

# By module get header
HEADERS_BUILD := \
	$(patsubst %,$(SRCGEN_DIR)/include/module/%.h,$(subst .,/, \
		$(foreach i,$(MODS_ENABLE_OBJ), \
			$(call get,$(call get,$i,type),qualifiedName)) $(APIS_BUILD)))

$(HEADERS_BUILD) : %.h : %.h.tmp ;

$(HEADERS_BUILD:%=%.tmp) : mk/image.mk $(configfiles_model_mk)
	@$(MKDIR) $(@D) && printf "%b" '$(call __header_gen,$(basename $@))' > $@
	@diff -q $@ $(basename $@) >/dev/null 2>&1 \
	|| (cp $@ $(basename $@); echo Module header $(basename $@))

$(OPTION_CHECK_MK) : $(OPTION_CHECK_H)
	@$(CPP) -P $< > $@

$(OPTION_CHECK_H) : $(build_model_mk)
	@$(MAKE) -f mk/script/option_check.mk > $@
