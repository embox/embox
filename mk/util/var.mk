#
# Variable assignment utils.
#
# Note: The most functions here are highly optimized using inlining, make
# version-specific features, and so on.
# This is achieved at the expense of the code readability and simplicity,
# so it is assumed that the reader will not dig into the internals of how does
# it work. You've been warned.
# If you anyway want to understand how it works, there are some cheats at the
# bottom of this file. ;-)
#
# Author: Eldar Abusalimov
#

#
# Expands to the variable information string.
# Information includes flavor, origin and value of the specified variable.
#
# Params:
#  1. The target variable name
#
# Returns: Human-readable multiline string containing variable information.
#
define var_dump
$(call assert_called,var_dump,$0) \
  $(call assert,$1,Variable name is empty) Variable [$1] info:
   flavor: [$(flavor $1)]
   origin: [$(origin $1)]
    value: [$(value $1)]

endef

#
# Prints the information about the specified variable using $(info) function.
#
# Params:
#  1. The target variable name
#
# Returns: nothing
#
# See also: var_dump
#
var_info = $(call assert_called,var_info,$0)$(info $(call var_dump,$1))

#
# Function: var_assign_recursive, =
#
# Assigns value to the specified variable, the variable becomes recursively
# expanded.
# This function is backed either by var_assign_singleline_recursive or by
# var_assign_multiline_recursive, the proper choice is made by checking for
# newline character $(\n) in the value being assigned.
#
# Params:
#  1. The target variable name
#  2. The value to assign
#
# Returns: nothing
#
# Note: the underlying implementation uses $(eval) function for the assignment,
#       and all notes about its arguments are applied for this case too
#       (see GNU Make manual).
#
# See also: var_assign_singleline_recursive var_assign_multiline_recursive
#

#
# Function: var_assign_singleline_recursive
#
# Assigns value to the specified variable, the variable becomes recursively
# expanded. The value must not contain any newline characters.
#
# Params:
#  1. The target variable name
#  2. The value to assign
#
# Returns: nothing
#
# Note: the underlying implementation uses $(eval) function for the assignment,
#       and all notes about its arguments are applied for this case too
#       (see GNU Make manual).
#
# Note: all that this function does is just evaluates something like:
#       $1 = $2
#

#
# Function: var_assign_simple, =:
#
# Assigns value to the specified variable using immediate expansion (variable
# flavor becomes simple).
#
# Params:
#  1. The target variable name
#  2. The value to assign
#
# Returns: nothing
#
# Note: the underlying implementation uses $(eval) function for the assignment,
#       and all notes about its arguments are applied for this case too
#       (see GNU Make manual).
#

var_assign_undefined
var_assign_recursive
var_assign_simple
var_assign_append
var_assign_cond
=
=:
=+
=?
var_assign_singleline_recursive
var_assign_singleline_simple
var_assign_singleline_append
var_assign_singleline_cond
var_assign_multiline_recursive
var_assign_multiline_simple
var_assign_multiline_append
var_assign_multiline_cond

# Here goes the implementation.

# Include version-specific file.
#
# It have to define __var_assign_<line>_<op>_mk variables (8 ones total),
#     where <line> is one of these values:
__var_assign_lines := singleline multiline
#       and <op> is:
__var_assign_ops   := recursive simple append cond
# and __var_assign_undefined_mk variable.
#
# Thus:
__var_assign_lines_ops := $(sort undefined \
  $(foreach line,$(__var_assign_lines), \
    $(foreach op,$(__var_assign_ops), \
      $(line)_$(op) \
    ) \
  ) \
)
#
# __var_assign_inline list should be populated with <line>_<op> values
# for which var_assign_<line>_<op> variables could be inlined when being
# constructed from the corresponding __var_assign_<line>_<op>_mk ones.
__var_assign_inline :=

# And some definitions used for ops shorthands and possibly used by internals.
__var_assign_op_recursive :=  =
__var_assign_op_simple    := :=
__var_assign_op_append    := +=
__var_assign_op_cond      := ?=

# Well, include the implementation.
__var_impl_file = \
  $(self_makefile_dir)var-$(if $(call make_version_gte,3.82),3.82,3.81).mk
include $(__var_impl_file)

# Message to barf if something goes wrong.
__var_impl_you_do_this_wrong_msg = \
  Check your implementation ($(__var_impl_file))

# Implementation sanity check (to make sure that all needed has been defined).
__var_assign_mk_missing := $(strip \
  $(foreach def,$(__var_assign_lines_ops:%=__var_assign_%_mk), \
    $(if $(filter undefined,$(origin $(def))),$(def)) \
  ) \
)
$(call assert,$(call not,$(__var_assign_mk_missing)),Missing def: \
  $(__var_assign_mk_missing). $(__var_impl_you_do_this_wrong_msg))

# Implementation sanity check (values of __var_assign_inline list).
__var_assign_inline_bogus := \
  $(filter-out $(__var_assign_lines_ops),$(__var_assign_inline))
$(call assert,$(call not,$(__var_assign_inline_bogus)),Invalid values \
  in __var_assign_inline: $(__var_assign_inline_bogus). \
  $(__var_impl_you_do_this_wrong_msg))

__var_assign_lines_ops_inline := \
  $(sort $(filter $(__var_assign_lines_ops),$(__var_assign_inline)))

__var_assign_lines_ops_noinline := \
  $(sort $(filter-out $(__var_assign_inline),$(__var_assign_lines_ops)))

ifndef MK_UTIL_VAR_NO_INLINE
__var_assign_def_inline = ${eval $ \
  define $1$(\n)$             \
  $${eval $(value $2)}$(\n)$  \
  endef$                      \
}
else
__var_assign_def_inline = __var_assign_def_noinline
endif
__var_assign_def_noinline = ${eval $1 = $${eval $$($2)}}

$(foreach xline,inline noinline, \
  $(foreach def,$(__var_assign_lines_ops_$(xline)), \
    $(call __var_assign_def_$(xline),var_assign_$(def),__var_assign_$(def)_mk) \
  ) \
)

__var_assign_autoline_def = ${eval $ \
  $1 = $${eval $$(if $$(findstring $$(\n),$$2),$ \
    $$(__var_assign_multiline_$2_mk),$ \
    $$(__var_assign_singleline_$2_mk)$ \
  )}$ \
}

__var_assign_op_escaped_name = =$(subst =,,$(__var_assign_op_$2))
$(foreach op,$(__var_assign_ops), \
  $(foreach def,var_assign_$(op) $$(__var_assign_op_escaped_name), \
    $(call __var_assign_autoline_def,$(def),$(op)) \
  ) \
)

# Yes, everything above may look horrible, but it is only at first sight.
# Enabling the following block may help you to understand what is happening
# when one or another function is being expanded.
#  -- Eldar
ifeq (1,0)
__var_info_simplified = $(info $1 = $(value $(strip $1))$(\n))
$(call __var_info_simplified,var_assign_undefined)
$(call __var_info_simplified,var_assign_recursive)
$(call __var_info_simplified,var_assign_simple   )
$(call __var_info_simplified,var_assign_append   )
$(call __var_info_simplified,var_assign_cond     )
$(call __var_info_simplified,= )
$(call __var_info_simplified,=:)
$(call __var_info_simplified,=+)
$(call __var_info_simplified,=?)
$(call __var_info_simplified,var_assign_singleline_recursive)
$(call __var_info_simplified,var_assign_singleline_simple   )
$(call __var_info_simplified,var_assign_singleline_append   )
$(call __var_info_simplified,var_assign_singleline_cond     )
$(call __var_info_simplified,var_assign_multiline_recursive)
$(call __var_info_simplified,var_assign_multiline_simple   )
$(call __var_info_simplified,var_assign_multiline_append   )
$(call __var_info_simplified,var_assign_multiline_cond     )
$(call __var_info_simplified,__var_assign_singleline_recursive_mk)
$(call __var_info_simplified,__var_assign_singleline_simple_mk   )
$(call __var_info_simplified,__var_assign_singleline_append_mk   )
$(call __var_info_simplified,__var_assign_singleline_cond_mk     )
$(call __var_info_simplified,__var_assign_multiline_recursive_mk)
$(call __var_info_simplified,__var_assign_multiline_simple_mk   )
$(call __var_info_simplified,__var_assign_multiline_append_mk   )
$(call __var_info_simplified,__var_assign_multiline_cond_mk     )
endif

# XXX rewrite everything below. -- Eldar

var_swap = $(call __var_swap,$1,$2,__var_swap_tmp)
__var_swap = $ \
  $(call var_assign_$(flavor $1),$3,$(value $1))$ \
  $(call var_assign_$(flavor $2),$1,$(value $2))$ \
  $(call var_assign_$(flavor $3),$2,$(value $3))

var_save = $(strip \
  $(call assert_called,var_save,$0) \
  $(call set,__var_value,$1,$(value $1)) \
  $(call set,__var_flavor,$1,$(flavor $1)) \
)
var_restore = $(strip \
  $(call assert_called,var_restore,$0) \
  $(call var_assign_$(call get,__var_flavor,$1),$1,$(call get,__var_value,$1))\
)

