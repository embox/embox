.PHONY : all __extbld FORCE
all : __extbld

__extbld :
	@$(if $^,echo 'External build complete',:)

FORCE :

include mk/core/common.mk
include $(MKGEN_DIR)/build.mk
include mk/flags.mk # It must be included after a user-defined config.

.SECONDEXPANSION:

include $(MKGEN_DIR)/include.mk
include $(filter %.extbld_rule.mk,$(__include))

common_prereqs_nomk  = mk/extbld.mk mk/flags.mk $(MKGEN_DIR)/build.mk
common_prereqs       = $(common_prereqs_nomk) $(mk_file)
extbld_prerequisites = $(common_prereqs)

