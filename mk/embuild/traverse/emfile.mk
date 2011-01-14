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
# Returns: error entry
emfile_error = \
  $(patsubst %,$(emfile_error_pattern), \
    $(call log_error_entry,emfile,$(__emfile),[emfile] $1))
emfile_error_pattern = error(%)
emfile_errors_extract = \
  $(patsubst $(emfile_error_pattern),%,$(emfile_errors_filter))
emfile_errors_filter = \
  $(filter $(emfile_error_pattern),$1)

# Wraps entity so that it could be differed from e.g. error entries.
# Params:
#  1. entity
# Returns: entity entry
emfile_entity = \
  $(1:%=$(emfile_entity_pattern))
emfile_entity_pattern = entity(%)
emfile_entities_extract = \
  $(patsubst $(emfile_entity_pattern),%,$(emfile_entities_filter))
emfile_entities_filter = \
  $(filter $(emfile_entity_pattern),$1)

# Returns:
#   'single'   for *.em, and
#   'multiple' for Embuild
emfile_type = $(call __emfile_type,$(notdir $(__emfile)))
__emfile_type = \
  $(if $(call singleword,$1),$ \
    $(if $(filter Embuild,$1),single,$ \
       $(if $(filter %.em,$1),multiple,$ \
         $(error invalid emfile: __emfile is [$(__emfile)]))))

# Returns:
#   basename for *.em files, and
#   empty string for Embuild
emfile_name = $(basename $(filter %.em,$(notdir $(__emfile))))

#
# Steps:
#  filter
#  check
#  define
#

emfile_chain = $(call __emfile_chain,$(strip $1),)
__emfile_chain = \
  $(if $1, \
    $(call $0,$(call list_nofirst,$1), \
               $(call __emfile_chain_invoke,$(call first,$1),$2)), \
    $2 \
)

__emfile_chain_invoke = \
  $(call emfile_errors_filter,$2) $(call $1,$(call emfile_entities_extract,$2))

emfile_handle_all = $(strip \
  $(call emfile_handle_chain_results, \
      $(call emfile_chain, \
             emfile_chain_handle_filter_sandbox_variables \
             emfile_chain_handle_check_entities_have_unique_names \
             emfile_chain_handle_check_entities_are_named_in_place \
       ) \
   ) \
)

emfile_handle_chain_results = \
  $(or $(strip $(call emfile_errors_filter,$1)), \
       $(call emfile_define_entities,$(call emfile_entities_extract,$1)))


######### Filtering

emfile_chain_handle_filter_sandbox_variables = \
  $(call var_filter_out, \
          $(__emfile_sandbox_variables_before), \
          $(__emfile_sandbox_variables_after), \
              emfile_filter_handle)

# Called for each user defined variable. Detects double word variable names
# and tries to interpret them as entities.
# Params:
#  1. Variable name as is
# Returns: valid entity corresponding for given variable name or error entry
emfile_filter_handle = \
  $(if $(filter-out 2,$(words $1)), \
    $(call emfile_error,Invalid em-file variable: $1), \
    $(call emfile_filter_try_entity,$(word 1,$1),$(word 2,$1),$1) \
  )

# Called for each double word named variable by emfile_filter_handle.
# Such variable is a candidate to be an entity. Performs the necessary checks
# for entity type and name, and passes them to the next checker function
# on success.
# Params:
#  1. First word of the variable name (considered as entity type)
#  2. Second word of the variable name (considered as entity name)
#  3. Variable name as is
# Returns: entity entry if recognized a valid one, error entry otherwise
emfile_filter_try_entity = \
  $(or $(call emfile_errors_filter, \
           $(call emfile_filter_try_entity_type,$1,$3) \
           $(call emfile_filter_try_entity_name,$2,$3)), \
       $(call emfile_filter_entity_checked,$(call entity,$1,$2),$3))

# Params:
#  1. Word considered as entity type
#  2. Variable name as is
# Returns: given type on success, error entry on failure
emfile_filter_try_entity_type = \
  $(or $(call entity_check_type,$1), \
       $(call emfile_error,Invalid em-file variable: $2. \
                        '$1' is not recognized as a valid entity type))

# Params:
#  1. Word considered as entity name
#  2. Variable name as is
# Returns: given name on success, error entry on failure
emfile_filter_try_entity_name = \
  $(or $(call entity_check_name,$1), \
       $(call emfile_error,Invalid em-file variable: $2. \
                        '$1' is not a valid entity name))

# Called for each valid entity (with type and name checked).
# Params:
#  1. entity
#  2. Variable name as is
# Returns: entity entry
emfile_filter_entity_checked = \
  $(call emfile_entity,$(call emfile_filter_entity_assign_variable_name,$1,$2))

# For given entity defines a variable which will hold the name of user defined
# variable.
# Params:
#  1. entity
#  2. User defined variable name as is
# Returns: the first argument.
emfile_filter_entity_assign_variable_name = \
  $(call var_assign_simple \
    ,$(1:%=$(emfile_filter_entity_variable_name_pattern)),$2)$1

# Pattern to construct the name of the variable which expands to the name
# of user defined variable as is (e.g. 'module    foo').
emfile_filter_entity_variable_name_pattern := \
  __emfile_entity_variable_name-%

######### Checking

# Params:
#  1. all entities
# Return: Entity entries for properly named ones,
#          error entries for the rest (if there are name conflicts).
emfile_chain_handle_check_entities_have_unique_names = \
  $(call emfile_check_names,$(sort $(call entity_names,$1)),$1)

# Params:
#  1. entity names with duplicates removed
#  2. all entities
# Return: Entity entries for properly named ones,
#          error entries for the rest (if there are name conflicts).
emfile_check_names = \
  $(if $(filter-out $(words $1),$(words $2)), \
    $(call emfile_check_names_conflicting,$1,$2), \
    $(call emfile_entity,$2) \
   )

# Things are bad. Assumed to be cold, feel free.
# Params:
#  1. entity names with duplicates removed
#  2. all entities
# Return: Entity entries for properly named ones, error entries for the rest.
emfile_check_names_conflicting = \
  $(foreach e_name,$1,$(call emfile_check_name_is_unique \
         ,$(e_name),$(call entity_types_for_name,$(e_name),$2)))

# Params:
#  1. entity name
#  2. all entity types for the name (probably with duplicates)
# Return: Valid entity entry if all is ok, error entry if conflict is detected.
emfile_check_name_is_unique = \
  $(if $(call list_single,$2), \
    $(call emfile_entity,$(call entity,$1,$2)), \
    $(call emfile_check_name_unique_error,$1,$2,$(sort $2)) \
   ) \

# Params:
#  1. entity name
#  2. all entity types for the name (probably with duplicates)
#  3. entity types with no duplicates
# Return: Error entry about naming conflict.
emfile_check_name_unique_error = \
  $(call emfile_error,Entity named '$1' defined more than once: \
        $(foreach e_type,$(call chop,$3),$ \
            $(call __emfile_error_str_x_times_as_type,$(e_type),$2)$(\comma)) \
        $(call __emfile_error_str_x_times_as_type,$(call last,$3),$2))
__emfile_error_str_x_times_as_type = \
  $(call __emfile_error_str_x_times,$(words $(filter $1,$2))) as '$1'
__emfile_error_str_x_times = \
  $(if $(filter 1,$1),once,$(if $(filter 2,$1),twice,$1 times))

# Checks for acceptability to define the specified entity in the particular
# emfile (whether entity named 'foo' is defined in foo.em or in Embuild).
# Params:
#  1. entities list
# Returns: entity entries for good ones, and error entries for the rest.
emfile_chain_handle_check_entities_are_named_in_place = \
  $(foreach e,$1, \
    $(if $(call emfile_check_entity_named_in_place_$(emfile_type),$e), \
      $(call emfile_entity,$e), \
      $(call emfile_error,$(call entity_print,$e) can be defined either in \
        $(call entity_name,$e).em or in Embuild, not in $(notdir $(__emfile)))\
    ) \
  )
emfile_check_entity_named_in_place_single = \
  $(filter $(call entity_name,$1),$(emfile_name))
emfile_check_entity_named_in_place_multiple = $(true)

##### Define

emfile_define_entities = \
  $(foreach e,$1,$(call emfile_define_entity,$e))

# Params:
#  1. entity
emfile_define_entity = \
  $(call emfile_define_entity_entry \
       ,$(emfile_entity),$(emfile_define_get_entity_value))

# Params:
#  1. entity entry
#  2. entity value
# Returns: the given argument
emfile_define_entity_entry = \
  $(call var_assign_simple,$1,$2)$1

# Params:
#  1. entity
# Returns: value of user defined variable corresponding to the entity.
emfile_define_get_entity_value = \
  $(value $(emfile_define_get_entity_variable_name))

# Params:
#  1. entity
# Returns: variable name to get the original name of the variable corresponding
#          to the specified entity.
emfile_define_get_entity_variable_name = \
  $($(1:%=$(emfile_filter_entity_variable_name_pattern)))

endif # __embuild_traverse_emfile_mk

