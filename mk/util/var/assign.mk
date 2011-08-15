#
# Copyright 2008-2010, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2008-2010, Lanit-Tercom Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

##
# Variable manipulation utils.
#
# Author: Eldar Abusalimov
#

ifndef __util_var_assign_mk
__util_var_assign_mk := 1

# Implementation note:
#  There was an attempt to support all assignment ops and to abstract from
#  make version.
#  At this point I think it was unnecessary. If some day I'll change my
#  opinion, the previous solution can be found in embuild_experimental @ r895.

include mk/core/common.mk

##
# Function: var_assign_simple, var:=
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
# Example:
#  $(call var_assign_simple,foo,$$(bar))
#  $(call var_info,foo) # value is $(bar), flavor is simple
#  $(info $(foo)) # the expansion is $(bar) (dollar and 'bar' in braces)
#
var_assign_simple = \
  ${eval $$1 := $$2}

##
# Function: var_assign_recursive
#
# Assigns value to the specified variable, the variable becomes recursively
# expanded.
# This function is backed either by var_assign_recursive_sl or by
# var_assign_recursive_ml, the proper choice is made by checking for
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
# Example:
#  $(call var_assign_recursive,foo,$$(bar))
#  $(call var_info,foo) # value is $(bar), flavor is recursive
#  $(info $(foo)) # the expansion is expansion of variable 'bar'
#
# See also: var_assign_recursive_sl var_assign_recursive_ml
#
var_assign_recursive = \
  $(var_assign_recursive_$(if $(findstring $(\n),$2),ml,sl))

##
# Function: var_assign_recursive_sl
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
var_assign_recursive_sl = \
  ${eval $$1 = $(subst $(\h),\$(\h),$2)}

##
# Function: var_assign_recursive_ml
#
# Assigns value to the specified variable, the variable becomes recursively
# expanded. The value should meet requirements specified for 'define'
# directive, partularly, any nested 'define' must be balanced by the
# corresponding 'endef'.
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
${eval $ \
  define var_assign_recursive_ml$(\n)$ \
  $${eval define $$$$1$(\n)$$2$(\n)endef}$(\n)$ \
  endef$                                      \
} # eval above is 3.81 bug (Savannah #27394) workaround.

##
# Function: var_assign_undefined, var_undefine
#
# Undefines variable with the specified name.
#
# Params:
#  1. The variable to undefine
#
# Returns: nothing
#
# Example:
#  foo := bar
#  $(call var_undefine,foo)
#  $(call var_info,foo) # value is <empty>, flavor is undefined
#                       # (well, it is not definitely so, because
#                       # true undefine has been introduced only in 3.82)
#
ifneq ($(filter 3.80 3.81,$(MAKE_VERSION)),)
# This is not true undefine, but
# at least "ifdef" conditionals will behave as expected.
var_assign_undefined = \
  $(if $(filter-out undefined,$(flavor $1)),${eval $$1 =})
else
# Since version 3.82 GNU Make provides native 'undefine' directive.
var_assign_undefined = \
  ${eval undefine $$1}
endif

var_assign_simple_append = \
  $(if $(findstring simple,$(flavor $1)),${eval $$1 += $$2},${eval $$1 := \
        $(if $(findstring recursive,$(flavor $1)),$$($$1) )$$2})

var_assign_simple_remove = \
  $(if $(findstring %,$2),$(__var_assign_simple_remove_escaped)$ \
      ,${eval $$1 := $$(filter-out $$2,$$($$1))}
__var_assign_simple_remove_escaped = \
  $(error __var_assign_simple_remove_escaped not yet implemented)

var_assign_simple_append_unique = \
  $(if $(findstring %,$2),$(__var_assign_simple_append_unique_escaped)$ \
      ,$(if $(findstring undefined,$(flavor $1)),${eval $$1 := $$2},$ \
           $(if $(filter $2,$($1)),,$(if $(findstring simple,$(flavor $1)) \
               ,${eval $$1 += $$2},${eval $$1 := $$($$1) $$2}))))
__var_assign_simple_append_unique_escaped = \
  $(error __var_assign_simple_append_unique_escaped not yet implemented)

# Shorthand versions of var_assign_xxx functions.
$(call var_assign_recursive,var:=,$(value var_assign_simple))
$(call var_assign_recursive,var+=,$(value var_assign_simple_append))
$(call var_assign_recursive,var-=,$(value var_assign_simple_remove))
$(call var_assign_recursive,var*=,$(value var_assign_simple_append_unique))
$(call var_assign_recursive,var?=,$(value var_assign_simple_conditional))
$(call var_assign_recursive,var_undefine,$(value var_assign_undefined))

endif # __util_var_assign_mk
