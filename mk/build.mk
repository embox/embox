#
#   Date: Apr 4, 2012
# Author: Anton Kozlov
#

include mk/configure.mk #FIXME
include mk/codegen-dot.mk

include mk/help-module.mk

.PHONY : build image prepare docsgen dot

build : image
	@echo 'Build complete'

image : prepare

include mk/image.mk #FIXME move it upper. -- Eldar

prepare:
	@$(MKDIR) $(BUILD_DIR)
	@$(MKDIR) $(BIN_DIR)
	@$(MKDIR) $(OBJ_DIR)
	@$(MKDIR) $(LIB_DIR)
	@$(MKDIR) $(ROOTFS_DIR)
	@$(MKDIR) $(AUTOCONF_DIR)
	@$(MKDIR) $(DOCS_OUT_DIR)

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


mod-brief-% :
	@echo There is no such module in build
