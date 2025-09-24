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

ifneq ($(filter distgen,$(MAKECMDGOALS)),)
export GEN_DIST := 1
endif

.PHONY : buildgen distgen build docsgen dot

build_gen_ts := $(BUILD_DIR)/build-gen.timestamp

build : $(build_gen_ts)
	@$(MAKE) -f mk/script/build/oldconf-gen.mk MAKEFILES=''
	@$(MAKE) -f mk/board_conf/board-conf-gen.mk MAKEFILES=''
	@$(MAKE) -f mk/script/user-lds-sections-symbols.mk > $(SRCGEN_DIR)/section_symbols.lds.h
	@$(MAKE) -f mk/script/lds-apps.mk > $(SRCGEN_DIR)/apps.lds.h
	@$(MAKE) -f mk/script/incinst.mk
	@$(MAKE) -f mk/extbld/toolchain.mk MAKEFILES=''
	@$(MAKE) -f mk/extbld.mk MAKEFILES='' __extbld-1
	@$(MAKE) -f mk/image2.mk MAKEFILES='' STAGE=1
	@$(MAKE) -f mk/extbld/toolchain.mk do_test
	@$(MAKE) -f mk/extbld.mk MAKEFILES='' __extbld-2
	@$(MAKE) -f mk/image2.mk MAKEFILES='' STAGE=2
	@$(MAKE) -f mk/image3.mk MAKEFILES=''

distgen : $(build_gen_ts)
	@$(MAKE) -f mk/script/build/oldconf-gen.mk MAKEFILES=''
	@$(MAKE) -f mk/board_conf/board-conf-gen.mk MAKEFILES=''
	@$(MAKE) -f mk/script/user-lds-sections-symbols.mk > $(SRCGEN_DIR)/section_symbols.lds.h
	@$(MAKE) -f mk/script/lds-apps.mk > $(SRCGEN_DIR)/apps.lds.h
	@$(MAKE) -f mk/script/incinst.mk
	@$(MAKE) -f mk/extbld/toolchain.mk MAKEFILES=''
	@$(MAKE) -f mk/extbld.mk MAKEFILES='' __extbld-1
	@$(MAKE) -f mk/image2.mk MAKEFILES='' STAGE=1
	@$(MAKE) -f mk/extbld/toolchain.mk do_test
	@$(MAKE) -f mk/extbld.mk MAKEFILES='' __extbld-2
	@$(MAKE) -f mk/image2.mk MAKEFILES='' STAGE=2

buildgen: $(build_gen_ts)
ifneq ($(filter buildgen,$(MAKECMDGOALS)),)
.PHONY : $(build_gen_ts)
endif

$(build_gen_ts) : mk/script/build/build-gen.mk $(load_mybuild_files)
	@echo ' BUILDGEN $(DIST_DIR)'
	@$(MAKE) -f mk/script/build/oldconf-gen.mk MAKEFILES=''
	@$(MAKE) -f $< MAKEFILES='$(MKGEN_DIR)/build.mk $(MAKEFILES)'
	@$(MAKE) -f mk/extbld/toolchain.mk MAKEFILES=''
	@$(MAKE) -f mk/script/incinst.mk
	@$(MKDIR) $(@D) && touch $@

# force regeneration of build files when some of them are missing

-include $(MKGEN_DIR)/include.mk
__include ?=

ifneq ($(words $(__include)), $(words $(wildcard $(__include))))
.PHONY : $(build_gen_ts)
endif

docsgen:
	@[ -d $(DOCS_OUT_DIR) ] || $(MKDIR) $(DOCS_OUT_DIR)
	doxygen mk/Doxyfile
	@echo 'Docs generation complete'

MODULE_LIST := $(strip $(call mod_list))

mod-list :
	$(info --- Module list --- )
	@$(info $(addsuffix $(\n),$(MODULE_LIST)))#

$(MODULE_LIST:%=mod-include-reason-%) : mod-include-reason-% :
	$(call mod_include_reason,$*)#

$(MODULE_LIST:%=mod-brief-%) : mod-brief-% :
	@$(info $(call mod_brief,$*))#


mod-brief-% mod-include-reason-%:
	@echo There is no $* module in build
