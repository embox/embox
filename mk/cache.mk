
ifndef CACHE_INCLUDES
$(error CACHE_INCLUDES is not defined)
endif

ifeq ($(findstring --no-print-directory,$(MAKEFLAGS)),)
$(error '--no-print-directory' flag must be specified)
endif

.PHONY : all
all :
	$(__cache_print_all_new_variable_definitions)
	@echo \# done

include mk/core/common.mk

__cache_preinclude_variables :=
__cache_postinclude_variables :=

__cache_escape_variable_name = \
	$(subst $(=),$$(=),$(subst $(:),$$(:),$(subst $(\h),$$(\h),$(subst $$,$$$$,$1))))
__cache_variable_has_leading_ws = \
	$(subst x$(firstword $(value $1)),,$(firstword x$(value $1)))

# __cache_construct_xxx
# Arg 1: variable name.
__cache_construct_variable_definition = \
	$(__cache_construct_$(if \
		$(findstring $(\h),$(subst $(\n),$(\h),$(value $1))) \
			,verbose,regular)_$(flavor $1)_variable)

__cache_construct_regular_simple_variable = \
	$(__cache_escape_variable_name) := \
		$(if $(__cache_variable_has_leading_ws),$$(\0))$(subst $$,$$$$,$(value $1))

__cache_construct_regular_recursive_variable = \
	$(if $(__cache_variable_has_leading_ws),$ \
		$(__cache_construct_verbose_recursive_variable),$ \
		$(__cache_escape_variable_name) = $(value $1))

__cache_construct_verbose_recursive_variable = \
	define $(__cache_escape_variable_name)$(\n)$ \
		$(value $1)$(\n)$ \
	endef
__cache_construct_verbose_simple_variable = \
	$(__cache_construct_verbose_recursive_variable)$(\n)$ \
	$(__cache_escape_variable_name) := $$(value $(__cache_escape_variable_name))

# No args.
__cache_print_all_new_variable_definitions = \
	$(and $(foreach 1,$(sort $(filter-out \
			$(__cache_preinclude_variables), \
			$(__cache_postinclude_variables))), \
		$(info $(__cache_construct_variable_definition))),)

# Collect variables...

__cache_preinclude_variables := $(.VARIABLES)
include $(CACHE_INCLUDES)
__cache_postinclude_variables := $(.VARIABLES)

