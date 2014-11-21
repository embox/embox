#
#   Date: Nov 17, 2014
# Author: Eldar Abusalimov
#

.PHONY : build docsgen

build :
	@$(MAKE) -f mk/extbld.mk MAKEFILES='' __extbld-1
	@$(MAKE) -f mk/image2.mk MAKEFILES='' STAGE=1
	@$(MAKE) -f mk/extbld/toolchain.mk do_test
	@$(MAKE) -f mk/extbld.mk MAKEFILES='' __extbld-2
	@$(MAKE) -f mk/image2.mk MAKEFILES='' STAGE=2
	@$(MAKE) -f mk/image3.mk MAKEFILES=''

docsgen:
	@[ -d $(DOCS_OUT_DIR) ] || $(MKDIR) $(DOCS_OUT_DIR)
	doxygen
	@echo 'Docs generation complete'
