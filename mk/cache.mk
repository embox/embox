
ifndef CACHE_INCLUDES
$(error CACHE_INCLUDES is not defined)
endif

.PHONY : all
all :
	$(__cache_print_all_new_variable_definitions)
	@echo \# done

__cache_preinclude_variables :=
__cache_postinclude_variables :=

define __cache_new_line


endef
__cache_new_line := $(__cache_new_line)

__cache_hash := \#

# Arg 1: variable name.
__cache_construct_variable_definition = \
	$(__cache_construct_$(if $(findstring $(__cache_hash), \
			$(subst $(__cache_new_line),$(__cache_hash), \
				$(subst $(__cache_hash)$(firstword $(value $1)),, \
					$(firstword $(__cache_hash)$(value $1))) \
				$(value $1))) \
			,verbose,regular)_definition_for_$(flavor $1)_variable)

__cache_construct_regular_definition_for_recursive_variable = \
	$(subst $$,$$$$,$1) = $(value $1)
__cache_construct_regular_definition_for_simple_variable = \
	$(subst $$,$$$$,$1 := $(value $1))

__cache_construct_verbose_definition_for_recursive_variable = \
	define $(subst $$,$$$$,$1)$(__cache_new_line)$(value \
		$1)$(__cache_new_line)endef
__cache_construct_verbose_definition_for_simple_variable = \
	$(__cache_construct_verbose_definition_for_recursive_variable) \
	$(__cache_new_line)$(subst $$,$$$$,$1) := $$(value $(subst $$,$$$$,$1))

# No args.
__cache_print_all_new_variable_definitions = \
	$(and $(foreach 1,$(filter-out \
			$(__cache_preinclude_variables), \
			$(__cache_postinclude_variables)), \
		$(info $(__cache_construct_variable_definition))),)

__cache_preinclude_variables := $(.VARIABLES)
include $(CACHE_INCLUDES)
__cache_postinclude_variables := $(.VARIABLES)
