.PHONY : all __extbld-1 __extbld-2 FORCE
all :

__extbld-1 __extbld-2 :
	@$(if $^,echo 'External build complete',:)

FORCE :

include mk/flags.mk
include mk/image_lib.mk

export EMBOX_CROSS_COMPILE := $(CROSS_COMPILE)

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(filter %.extbld_rule.mk,$(__include))
