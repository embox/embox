#
# Invoked with all scripts preloaded by the bootstrap sript.
# See 'mk/load.mk' for details about environment variables available in the
# invocation context.
#
#   Date: Apr 4, 2012
# Author: Anton Kozlov
#

include mk/codegen-dot.mk

include mk/help-module.mk

.PHONY : buildgen build prepare docsgen dot

build_gen_ts := $(MKGEN_DIR)/build-gen.timestamp

build : $(build_gen_ts)
	@$(MAKE) -f mk/script/build/oldconf-gen.mk MAKEFILES=''
	@$(MAKE) -f mk/extbld.mk MAKEFILES=''
	@$(MAKE) -f mk/image2.mk MAKEFILES=''

buildgen : $(build_gen_ts)
ifneq ($(filter buildgen,$(MAKECMDGOALS)),)
.PHONY : $(build_gen_ts)
endif

$(build_gen_ts) : mk/script/build/build-gen.mk $(load_mybuild_files)
	@echo ' BUILDGEN'
	@$(MAKE) -f $< && touch $@

# force regeneration of build files when some of them are missing

-include $(MKGEN_DIR)/include.mk
__include ?=

ifneq ($(words $(__include)), $(words $(wildcard $(__include))))
.PHONY : $(build_gen_ts)
endif

docsgen:
	@[ -d $(DOCS_OUT_DIR) ] || $(MKDIR) $(DOCS_OUT_DIR)
	doxygen
	@echo 'Docs generation complete'

MODULE_LIST := $(strip $(call mod_list))

mod-list :
	$(info --- Module list --- )
	@$(info $(addsuffix $(\n),$(MODULE_LIST)))#

$(MODULE_LIST:%=mod-include-reason-%) : mod-include-reason-% :
	@$(info $(call mod_include_reason,$*))#

$(MODULE_LIST:%=mod-brief-%) : mod-brief-% :
	@$(info $(call mod_brief,$*))#


mod-brief-% mod-include-reason-%:
	@echo There is no $* module in build
