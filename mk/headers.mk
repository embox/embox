
# By module get header
HEADERS_BUILD := \
  $(patsubst %,$(OBJ_DIR)/mods/%.h,$(subst .,/,$(foreach i,$(MODS_ENABLE_OBJ),$(call get,$(call get,$i,type),qualifiedName))))

$(HEADERS_BUILD): mk/image.mk $(build_model_mk)
	@$(MKDIR) $(@D) && printf "%b" '$(__header_gen)' > $@.tmp
	@diff -q $@ $@.tmp &>/dev/null; \
		if [ ! 0 -eq $$? ]; then mv $@.tmp $@; echo Module header $@; \
			else rm $@.tmp; fi
