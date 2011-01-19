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
# GMSL Logical operators.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_logic_mk
__gmsl_logic_mk := 1

#
# not is defined in core/common.mk
#

# ----------------------------------------------------------------------------
# Function:  and
# Arguments: Two boolean values
# Returns:   Returns $(true) if both of the booleans are true
# ----------------------------------------------------------------------------
ifneq ($(__gmsl_have_and),$(true))
and = $(__gmsl_tr2)$(if $1,$(if $2,$(true),$(false)),$(false))
endif

# ----------------------------------------------------------------------------
# Function:  or
# Arguments: Two boolean values
# Returns:   Returns $(true) if either of the booleans is true
# ----------------------------------------------------------------------------
ifneq ($(__gmsl_have_or),$(true))
or = $(__gmsl_tr2)$(if $1$2,$(true),$(false))
endif

# ----------------------------------------------------------------------------
# Function:  xor
# Arguments: Two boolean values
# Returns:   Returns $(true) if exactly one of the booleans is true
# ----------------------------------------------------------------------------
xor = $(__gmsl_tr2)$(if $1,$(if $2,$(false),$(true)),$(if $2,$(true),$(false)))

# ----------------------------------------------------------------------------
# Function:  nand
# Arguments: Two boolean values
# Returns:   Returns value of 'not and'
# ----------------------------------------------------------------------------
nand = $(__gmsl_tr2)$(if $1,$(if $2,$(false),$(true)),$(true))

# ----------------------------------------------------------------------------
# Function:  nor
# Arguments: Two boolean values
# Returns:   Returns value of 'not or'
# ----------------------------------------------------------------------------
nor = $(__gmsl_tr2)$(if $1$2,$(false),$(true))

# ----------------------------------------------------------------------------
# Function:  xnor
# Arguments: Two boolean values
# Returns:   Returns value of 'not xor'
# ----------------------------------------------------------------------------
xnor =$(__gmsl_tr2)$(if $1,$(if $2,$(true),$(false)),$(if $2,$(false),$(true)))

endif # __gmsl_logic_mk
