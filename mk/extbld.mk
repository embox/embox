.PHONY : all __extbld-1 __extbld-2 FORCE
all :

__extbld-1 __extbld-2 :
	@$(if $^,echo 'External build complete',:)

FORCE :

include mk/image_lib.mk
include $(MKGEN_DIR)/build.mk

include mk/flags.mk # It must be included after a user-defined config.

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(filter %.extbld_rule.mk,$(__include))

