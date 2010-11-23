#
# Traverser sandbox for em-files.
#
#   Date: Nov 13, 2010
# Author: Eldar Abusalimov
#

ifndef __embuild_traverse_emfile_mk
__embuild_traverse_emfile_mk := 1

include util/common.mk
include util/var_filter.mk
include util/var_assign.mk
include util/log.mk

include embuild/traverse/entity.mk

# Wrapper around generic error logger.
# Params:
#  1. Error message
emfile_error = $(call log_error,emfile,$(__emfile),[emfile] $1)

# Returns:
#   'single'   for *.em, and
#   'multiple' for Embuild
emfile_type = $(call __emfile_type,$($(notdir $(__emfile))))
__emfile_type = \
  $(if $(filter 1,$(words $1)),$ \
    $(if $(filter Embuild,$1),common,$ \
       $(if $(filter %.em,$1),personal,$ \
         $(error invalid emfile: __emfile is [$(__emfile)]))))


emfile_handle_all_variables = $(strip \
  $(call emfile_handle_filtered_entities, \
    $(call var_filter_out, \
           $(__emfile_sandbox_variables_before), \
           $(__emfile_sandbox_variables_after), \
               emfile_handle_variable), \
   ) \
)

emfile_handle_filtered_entities = \
  $(foreach entity,$1, \
    $(if $(filter-out 1,$(words $(filter $(entity),$1))), \
      $(call emfile_error) \
     ) \
   )

# Called for each user defined variable by var_filter_out. Detects double word
# variable names and tries to interpret them as entities.
# Params:
#  1. Variable name as is
# Returns: valid entity corresponding for given variable name (if any)
emfile_handle_variable = \
  $(if $(filter-out 2,$(words $1)), \
    $(call emfile_error,Invalid em-file variable: $1), \
    $(call emfile_handle_variable_maybe_entity,$(word 1,$1),$(word 2,$1),$1) \
  )

# Called for each double word named variable by emfile_handle_variable.
# Such variable is a candidate to be an entity. Performs the necessary checks
# for entity type and name, and passes them to the next checker function
# on success.
# Params:
#  1. First word of the variable name (considered as entity type)
#  2. Second word of the variable name (considered as entity name)
#  3. Variable name as is
emfile_handle_variable_maybe_entity = \
  $(if $(call not,$(call entity_check_type,$1)), \
    $(call emfile_error,Invalid entity type: $1), \
    $(if $(call not,$(call entity_check_name,$2)), \
      $(call emfile_error,Invalid entity name: $2), \
      $(call emfile_handle_entity_type_name_checked,$1,$2,$3) \
    ) \
  )

# Called for each valid entity (with type and name checked). Checks for
# acceptability to define the specified entity in the particular emfile.
emfile_handle_entity_type_name_checked = \
  $(if $(call not,$(call emfile_entity_names_match,$2)), \
    $(call emfile_error,$1 $2 can be defined either in \
                        $2.em or in Embuild, not in $(notdir $(__emfile))), \
    $(call emfile_entity_variable_assign_and_echo,$(call entity,$1,$2),$3) \
  )

# Checks whether entity named 'foo' is defined in foo.em or in Embuild.
#  1. entity name
# Returns: entity name if it is good, and nothing otherwise.
emfile_entity_names_match = \
  $(if $(filter $1 Embuild,$(basename $(notdir $(__emfile)))),$1)

# Defines a variable for the specified entity and assigns the name of
# user defined variable to it.
# Params:
#  1. entity
#  2. User defined variable name as is
# Returns: the first argument.
emfile_entity_variable_assign_and_echo = \
  $(call var_assign_simple,$(1:%=$(emfile_entity_variable_name_pattern)),$2)$1

# Pattern to construct the name of the variable which expands to the name
# of user defined variable as is (e.g. 'module    foo').
emfile_entity_variable_name_pattern := \
  __emfile_entity_variable_name-%

# Params:
#  1. entity
# Returns: variable name to get the original name of the variable corresponding
#          to the specified entity.
emfile_entity_variable_name = \
  $($(1:%=$(emfile_entity_variable_name_pattern)))

# Params:
#  1. entity
# Returns: value of user defined variable corresponding to the entity.
emfile_entity_value = \
  $(value $(emfile_entity_variable_name))

# TODO invoke it later. -- Eldar
emfile_handle_all_variables := $(emfile_handle_all_variables)

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
	$(MAKE) -f mk/embuild/traverse/entity_sandbox.mk all \
		__EMBUILD_TRAVERSE_ENTITY_NAME=$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*

__embuild_traverse_emfile_module-%: \
  export __EMBUILD_TRAVERSE_ENTITY_NAME=$(dir $(__EMBUILD_TRAVERSE_EMFILE))/$*

__embuild_traverse_emfile_module-%: \
  export __EMBUILD_TRAVERSE_ENTITY_TYPE = module

__embuild_traverse_emfile_%: \
  export __EMBUILD_TRAVERSE_ENTITY_FILE = $(__EMBUILD_TRAVERSE_EMFILE_ROOT)/$(__EMBUILD_TRAVERSE_EMFILE)

$(__embuild_traverse_emfile_entity_targets): \
  export __EMBUILD_TRAVERSE_ENTITY = $(value $(@:__embuild_traverse_emfile_%=%))

endif # __embuild_traverse_emfile_mk

