#
#   Date: Feb 9, 2012
# Author: Eldar Abusalimov
#

include mk/core/alloc.mk

MY_CACHE_DIR := mk/.cache/my

MY_PATH := src/ platform/
MY_FILES := $(shell find . -depth \( -name Mybuild -o -name \*.my \) -print)

all_cached_myfiles := \
	$(patsubst $(abspath ./%),$(MY_CACHE_DIR)/files/%.mk, \
		$(abspath $(MY_FILES)))

cached_mybuild_model := $(MY_CACHE_DIR)/model.mk
cached_mybuild_model_myfiles := $(MY_CACHE_DIR)/model-myfiles.mk

-include $(cached_mybuild_model_myfiles)
mybuild_model_myfiles ?=

export mybuild_model_myfiles_added := \
	$(filter-out $(mybuild_model_myfiles),$(all_cached_myfiles))
export mybuild_model_myfiles_removed := \
	$(filter-out $(all_cached_myfiles),$(mybuild_model_myfiles))

ifneq ($(or $(mybuild_model_myfiles_added),$(mybuild_model_myfiles_removed)),)
.PHONY : $(cached_mybuild_model)
endif

$(MAKECMDGOALS) : $(cached_mybuild_model)
	@$(MAKE) -f mk/main.mk MAKEFILES='$<' $@

export all_cached_myfiles

$(cached_mybuild_model) : mk/load2.mk
$(cached_mybuild_model) : mk/script/mk-persist.mk
$(cached_mybuild_model) : $(all_cached_myfiles)
	@echo ' MYBUILD ...'
	@$(foreach f,$(mybuild_model_myfiles_added)   ,echo '  A $f';)#
	@$(foreach f,$*                               ,echo '  M $f';)#
	@$(foreach f,$(mybuild_model_myfiles_removed) ,echo '  D $f';)#
	@mkdir -p $(@D) && \
		$(MAKE) -f mk/script/mk-persist.mk MAKEFILES='$(mk_mybuild) $(all_cached_myfiles)' \
		PERSIST_OBJECTS='$$(call new,mybuild,$$(foreach f,$$(all_cached_myfiles),$$($$f)))' \
		PERSIST_REALLOC='my' \
		ALLOC_SCOPE='m' > $@
	@printf 'mybuild_model_myfiles := %b' '$(all_cached_myfiles:%=\\\n\t%)' \
		> $(cached_mybuild_model_myfiles)

$(all_cached_myfiles) : mk/load2.mk
$(all_cached_myfiles) : mk/script/mk-persist.mk
$(all_cached_myfiles) : $(MY_CACHE_DIR)/files/%.mk : ./%
	@echo ' MYFILE $<'
	@mkdir -p $(@D) && \
		$(MAKE) -f mk/script/mk-persist.mk MAKEFILES='$(mk_mybuild)' \
		PERSIST_OBJECTS='$$(call new,resource,$<)' \
		ALLOC_SCOPE='r$(call alloc,res)' > $@
	@echo '$$(lastword $$(MAKEFILE_LIST)) := .obj1r$(call alloc_last,res)' >> $@

