#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

$(warning Sandboxing entity $(EMBUILD_ENTITY))

variables_before :=
variables_after  :=

variables_before := $(.VARIABLES)
include $(SRC_DIR)/$(EMBUILD_ENTITY).em
variables_after  := $(.VARIABLES)

variables_sandboxed := $(filter-out $(variables_before),$(variables_after))

include util.mk

variable_dump = \
  define $$_$1\n$(subst $(\n),\n,$(subst \,\\,$(value $1)))\nendef
variable_dump_all = \
  $(foreach var,$1,$(call variable_dump,$(var))\n)

$(OBJ_DIR)/%.mk : $(SRC_DIR)/%.em
	@mkdir -p $(@D)
	@$(PRINTF) '# Auto-generated EMBuild entity. Do not edit.\n' > $@
	$(PRINTF) '$(call variable_dump_all,$(variables_sandboxed))' >> $@

