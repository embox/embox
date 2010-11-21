#
# Traverser sandbox for em-files.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

include util/common.mk
include util/var_filter.mk
include util/var_assign.mk
include util/log.mk
include embuild/traverse/entity.mk

emfile_error = $(call log_error,emfile,$(__emfile),[emfile] $1)

emfile_handle_variable = $(if $(filter 2,$(words $1)), \
  $(call emfile_variable_process_entity,$1,$(word 1,$1),$(word 2,$1)), \
  $(call emfile_variable_process_invalid,$1) \
)

emfile_handle_variable_entity = \
  $(if $(call not,$(call entity_type_check,$2)), \
    $(call emfile_error,Invalid entity type: $2), \
    $(if $(call not,$(call entity_name_check,$3)), \
      $(call emfile_error,Invalid entity name: $3), \
      $(call var_assign_simple,__emfile_entity_variable_$2-$3,$1)$2-$3 \
    ) \
  )

emfile_handle_variable_invalid = \
  $(call emfile_error,Invalid em-file variable: $1)

# TODO invoke it later. -- Eldar
emfile_handle_all_variables := $(strip \
  $(foreach v,$(call var_filter_out, \
                     $(__emfile_sandbox_variables_before), \
                     $(__emfile_sandbox_variables_after), \
                         emfile_handle_variable), \
    $(info Filtered: $v : [$(__emfile_entity_variable_$v)]) \
  ) \
)

#$(foreach e,$(filter __error_%_emfile,$(.VARIABLES)), \
  $(warning Recorded error $(e:__error_%_emfile=%): $($e)))

$(call log_report_by_tag,emfile)

$(error not yet implemented)

__embuild_traverse_emfile_entity_variables = \
  $(filter $(embuild_entity_types:%=%-%),$(emfile_variables))

# Can't detect type of the entity (bogus type/name).
__embuild_traverse_emfile_bogus_variables = \
  $(filter-out __embuild_traverse_emfile_entity_variables, \
           $(__embuild_traverse_emfile_variables))

__embuild_traverse_emfile_entity_targets := \
  $(__embuild_traverse_emfile_entity_variables:%=__embuild_traverse_emfile_%)

.PHONY: all
.PHONY: $(__embuild_traverse_emfile_entity_targets)
all: $(__embuild_traverse_emfile_entity_targets)

#$(__embuild_traverse_entity_types:%=__embuild_traverse_emfile_%-%):
$(__embuild_traverse_emfile_entity_targets): __embuild_traverse_emfile_module-%:
	$(MAKE) -f mk/embuild/traverse/entity.mk all \
		__EMBUILD_TRAVERSE_ENTITY_NAME=$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*

__embuild_traverse_emfile_module-%: \
  export __EMBUILD_TRAVERSE_ENTITY_NAME=$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*

__embuild_traverse_emfile_module-%: \
  export __EMBUILD_TRAVERSE_ENTITY_TYPE = module

__embuild_traverse_emfile_%: \
  export __EMBUILD_TRAVERSE_ENTITY_FILE = $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE)

$(__embuild_traverse_emfile_entity_targets): \
  export __EMBUILD_TRAVERSE_ENTITY = $(value $(@:__embuild_traverse_emfile_%=%))


