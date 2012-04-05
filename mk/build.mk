#
#   Date: Apr 4, 2012
# Author: Anton Kozlov
#

include mk/configure.mk #FIXME
include mk/codegen-dot.mk

include mk/help-module.mk

.PHONY : all image prepare docsgen dot

all : image

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

MODULE_LIST := $(strip $(call help_modulelist))

help-mod-list :
	$(info --- Module list --- )
	$(info $(addsuffix $(\n),$(MODULE_LIST)))
	@#

$(MODULE_LIST:%=help-mod-%) : help-mod-% :
	$(info $(call help_module,$*))
	@#

help-mod-% :
	@echo There is no such module in build
