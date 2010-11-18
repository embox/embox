#
# Traverser entity sandbox.
#
#   Date: Nov 15, 2010
# Author: Eldar Abusalimov
#

__embuild_traverse_entity_types := api module library package

ifdef __EMBUILD_TRAVERSE_ENTITY

include util/envdef.mk

$(call envdef_assert_defined, \
  __EMBUILD_TRAVERSE_ENTITY_NAME \
  __EMBUILD_TRAVERSE_ENTITY_TYPE \
  __EMBUILD_TRAVERSE_ENTITY_FILE \
  __EMBUILD_TRAVERSE_ENTITY
,Traverser entity sandbox needs these variables to be defined)

__embuild_traverse_entity_variables :=
__embuild_traverse_entity_variables := $(.VARIABLES)

# Go!
$(eval $(value __EMBUILD_TRAVERSE_ENTITY))

__embuild_traverse_entity_variables := \
  $(filter-out $(__embuild_traverse_entity_variables),$(.VARIABLES))

include util/common.mk

prereq = $(__EMBUILD_TRAVERSE_ENTITY_FILE)
target = $(OBJ_DIR)/$(__EMBUILD_TRAVERSE_ENTITY_NAME).mk

variable_dump = \
  define $$_$1\n$(subst $(\n),\n,$(subst \,\\,$(value $1)))\nendef
variable_dump_all = \
  $(foreach var,$1,$(call variable_dump,$(var))\n)

$(target) : $(prereq)
	@mkdir -p $(@D)
	@$(PRINTF) '# Auto-generated EMBuild entity. Do not edit.\n' > $@
	$(PRINTF) '$(call variable_dump_all,$(__embuild_traverse_emfile_variables))' >> $@

endif # __EMBUILD_TRAVERSE_ENTITY
