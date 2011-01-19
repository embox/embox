#
# Copyright 2008-2010, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2008-2010, Lanit-Tercom Inc. All rights reserved.
#
# Copyright 2005-2010 John Graham-Cumming
#
# Please see COPYRIGHT for details.
#

#
# GMSL Named stacks.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_stack_mk
__gmsl_stack_mk := 1

ifdef __gmsl_have_eval

# ----------------------------------------------------------------------------
# Function:  push
# Arguments: 1: Name of stack
#            2: Value to push onto the top of the stack (must not contain
#               a space)
# Returns:   None
# ----------------------------------------------------------------------------
push = $(__gmsl_tr2)$(call assert_no_dollar,$0,$1$2)$(eval __gmsl_stack_$1 := $2 $(if $(filter-out undefined,\
    $(origin __gmsl_stack_$1)),$(__gmsl_stack_$1)))

# ----------------------------------------------------------------------------
# Function:  pop
# Arguments: 1: Name of stack
# Returns:   Top element from the stack after removing it
# ----------------------------------------------------------------------------
pop = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(strip $(if $(filter-out undefined,$(origin __gmsl_stack_$1)), \
    $(call first,$(__gmsl_stack_$1))                                       \
    $(eval __gmsl_stack_$1 := $(call rest,$(__gmsl_stack_$1)))))

# ----------------------------------------------------------------------------
# Function:  peek
# Arguments: 1: Name of stack
# Returns:   Top element from the stack without removing it
# ----------------------------------------------------------------------------
peek = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(call first,$(__gmsl_stack_$1))

# ----------------------------------------------------------------------------
# Function:  depth
# Arguments: 1: Name of stack
# Returns:   Number of items on the stack
# ----------------------------------------------------------------------------
depth = $(__gmsl_tr1)$(call assert_no_dollar,$0,$1)$(words $(__gmsl_stack_$1))

endif # __gmsl_have_eval

endif # __gmsl_stack_mk
