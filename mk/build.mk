#
#   Date: Apr 4, 2012
# Author: Anton Kozlov
#

include mk/dirs.mk

include mk/configure.mk #FIXME

include mk/codegen-dot.mk

include mk/image.mk

.PHONY : docsgen dot prepare

all : image

image : prepare

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


