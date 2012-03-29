
# By module get header
HEADERS_BUILD := \
  $(patsubst %,$(OBJ_DIR)/mods/%.h,$(subst .,/,$(foreach i,$(MODS_ENABLE_OBJ),$(call get,$(call get,$i,type),qualifiedName))))

HEADERS_BUILD_TMP := $(HEADERS_BUILD:%=%.tmp)

$(HEADERS_BUILD) : %.h : %.h.tmp ;

$(HEADERS_BUILD_TMP) : mk/image.mk $(build_model_mk)
	@$(MKDIR) $(@D) && printf "%b" '$(call __header_gen,$(subst .tmp,,$@))' > $@
	@diff -q $@ $(subst .tmp,,$@) &>/dev/null || (cp $@ $(subst .tmp,,$@); echo Module header $@)
