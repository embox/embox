#
# Second stage: loads My-files and links them together.
#
#   Date: Feb 9, 2012
# Author: Eldar Abusalimov
#

MYBUILD_CACHE_DIR := mk/.cache/my
MYFILES_CACHE_DIR := mk/.cache/my/files

MYBUILD_PATH := src/ platform/ third-party/

MYFILES := \
	$(shell find $(MYBUILD_PATH) -depth \
		\( -name Mybuild -o -name \*.my \) -print)

mybuild_model_mk := $(MYBUILD_CACHE_DIR)/model.mk
myfiles_mk_cached_list_mk := $(MYBUILD_CACHE_DIR)/model-myfiles.mk

export myfiles_mk := \
	$(patsubst $(abspath ./%),$(MYFILES_CACHE_DIR)/%.mk, \
		$(abspath $(MYFILES)))

-include $(myfiles_mk_cached_list_mk)
myfiles_mk_cached ?=

export myfiles_mk_added := \
	$(filter-out $(myfiles_mk_cached),$(myfiles_mk))
export myfiles_mk_removed := \
	$(filter-out $(myfiles_mk),$(myfiles_mk_cached))

ifneq ($(or $(myfiles_mk_added),$(myfiles_mk_removed)),)
.PHONY : $(mybuild_model_mk)
endif

$(MAKECMDGOALS) : $(mybuild_model_mk)
	@$(MAKE) -f mk/main.mk MAKEFILES='$(all_mk_files) $<' $@

.DELETE_ON_ERROR:

$(myfiles_mk) : mk/load2.mk
$(myfiles_mk) : mk/script/mk-persist.mk
$(myfiles_mk) : $(MYFILES_CACHE_DIR)/%.mk : %
	@echo ' MYFILE $<'
	@SCOPE=`echo '$<' | sum | cut -f 1 -d ' '`; \
	mkdir -p $(@D) && \
	$(MAKE) -f mk/script/mk-persist.mk MAKEFILES='$(mk_mybuild)' \
		PERSIST_OBJECTS='$$(call new,resource,$<)' \
		ALLOC_SCOPE="r$$SCOPE" > $@ && \
	echo '$$(lastword $$(MAKEFILE_LIST)) := '".obj1r$$SCOPE" >> $@

$(mybuild_model_mk) : mk/load2.mk
$(mybuild_model_mk) : mk/script/mk-persist.mk
$(mybuild_model_mk) : $(myfiles_mk)
	@echo ' MYBUILD ...'
	@$(foreach f,$(myfiles_mk_added)   ,echo '  A $f';)#
	@$(foreach f,$*                     ,echo '  M $f';)#
	@$(foreach f,$(myfiles_mk_removed) ,echo '  D $f';)#
	@mkdir -p $(@D) && \
		$(MAKE) -f mk/script/mk-persist.mk MAKEFILES='$(mk_mybuild) $(myfiles_mk)' \
		PERSIST_OBJECTS='$$(call new,mybuild,$$(foreach f,$$(myfiles_mk),$$($$f)))' \
		PERSIST_REALLOC='my' \
		ALLOC_SCOPE='m' > $@
	@echo '__mybuild_model_instance := .my1m' >> $@
	@printf 'myfiles_mk_cached := %b' '$(myfiles_mk:%=\\\n\t%)' \
		> $(myfiles_mk_cached_list_mk)
