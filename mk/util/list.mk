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

# Most parts of this file are derived from GMSL.

#
# GNU Make Standard Library (GMSL)
#
# A library of functions to be used with GNU Make's $(call) that
# provides functionality not available in standard GNU Make.
#
# Copyright (c) 2005-2010 John Graham-Cumming
#
# This file is part of GMSL
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# Neither the name of the John Graham-Cumming nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#
# List manipulation functions.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __util_list_mk
__util_list_mk := 1

include util/__gmsl.mk

#
# Function:  first (same as LISP's car, or head)
# Arguments: 1: A list
# Returns:   Returns the first element of a list
#
first = $(firstword $1)

#
# Function:  last
# Arguments: 1: A list
# Returns:   Returns the last element of a list
#
ifeq ($(__gmsl_have_lastword),$(true))
last = $(lastword $1)
else
last = $(if $1,$(word $(words $1),$1))
endif

#
# Function:  rest (same as LISP's cdr, or tail)
# Arguments: 1: A list
# Returns:   Returns the list with the first element removed
#
rest = $(wordlist 2,$(words $1),$1)

#
# Function:  chop
# Arguments: 1: A list
# Returns:   Returns the list with the last element removed
#
chop = $(wordlist 2,$(words $1),x $1)

#
# Function:  map
# Arguments: 1: Name of function to $(call) for each element of list
#            2: List to iterate over calling the function in 1
#            3: Optional argument to pass when calling the function
# Returns:   The list after calling the function on each element
#
map = $(strip $(foreach a,$2,$(call $1,$a,$(value 3))))

#
# Function:  pairmap
# Arguments: 1: Name of function to $(call) for each pair of elements
#            2: List to iterate over calling the function in 1
#            3: Second list to iterate over calling the function in 1
#            4: Optional argument to pass when calling the function
# Returns:   The list after calling the function on each pair of elements
#
pairmap = $(strip \
  $(if $2$3,$(call $1,$(call first,$2),$(call first,$3),$(value 4)) \
         $(call $0,$1,$(call  rest,$2),$(call  rest,$3),$(value 4))))

#
# Function:  leq
# Arguments: 1: A list to compare against...
#            2: ...this list
# Returns:   Returns $(true) if the two lists are identical
#
leq = $(strip $(if $(call seq,$(words $1),$(words $2)),     \
          $(call __gmsl_list_equal,$1,$2),$(false)))

__gmsl_list_equal = $(if $(strip $1),                                       \
                        $(if $(call seq,$(call first,$1),$(call first,$2)), \
                            $(call __gmsl_list_equal,                       \
                                $(call rest,$1),                            \
                                $(call rest,$2)),                           \
                            $(false)),                                      \
                     $(true))

#
# Function:  lne
# Arguments: 1: A list to compare against...
#            2: ...this list
# Returns:   Returns $(true) if the two lists are different
#
lne = $(call not,$(call leq,$1,$2))

#
# Function:  reverse
# Arguments: 1: A list to reverse
# Returns:   The list with its elements in reverse order
#
reverse =$(strip $(if $1,$(call reverse,$(call rest,$1)) \
                        $(call first,$1)))

#
# Function:  uniq
# Arguments: 1: A list from which to remove repeated elements
# Returns:   The list with duplicate elements removed without reordering
#
uniq = $(strip $(if $1,$(call uniq,$(call chop,$1)) \
            $(if $(filter $(call last,$1),$(call chop,$1)),,$(call last,$1))))

#
# Function:  length
# Arguments: 1: A list
# Returns:   The number of elements in the list
#
length = $(words $1)

endif # __util_list_mk
