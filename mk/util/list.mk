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

##
# Function: list_length
# Gets the length of the specified list.
#
# Params:
#  1. The target list
# Returns: The number of elements in the list
#
list_length = \
  $(words $1)

##
# Function: list_empty
# Checks whether the specified list is empty.
#
# Params:
#  1. The target list
# Returns: True if the specified list is empty, false otherwise
#
list_empty = \
  $(call not,$(strip $1))

##
# Function: list_single
# Checks whether the specified list contains exactly one element.
#
# Params:
#  1. The target list
# Returns: True if the specified list has only one element, false otherwise
#
list_single = \
  $(call make_bool,$(filter 1,$(words $1)))

##
# Function: list_first
# Gets the first element of the specified list.
#
# Params:
#  1. The target list
# Returns: the first element of the list
#
list_first = \
  $(firstword $1)

##
# Function: list_last
# Gets the last element of the specified list.
#
# Params:
#  1. The target list
# Returns: the last element of the list
#
ifeq ($(lastword $(false) $(true)),$(true))
list_last = \
  $(lastword $1)
else
list_last = \
  $(if $(strip $1),$(word $(words $1),$1))
endif

##
# Function: list_nofirst
# Gets a list without the first element.
#
# Params:
#  1. The target list
# Returns: Returns the list with the first element removed
#
list_nofirst = \
  $(wordlist 2,$(words $1),$1)

##
# Function: list_nolast
# Gets a list without the last element.
#
# Params:
#  1. The target list
# Returns: Returns the list with the last element removed
#
list_nolast = \
  $(wordlist 2,$(words $1),x $1)

##
# Function: list_map
# Calls the specified function on each element of a list.
#
# Params:
#  1. Name of function to call for each element
#  2. List to iterate over calling the function
#  3. Optional argument to pass when calling the function
# Returns: The list after calling the function on each element
#
list_map = \
  $(strip $(foreach 2,$2,$(call $1,$2,$(value 3))))
# TODO $(foreach 2,$2,...) can be a bitch, check it. -- Eldar

#
# Function: list_pairmap
# Calls the specified function on each pair of elements of two lists.
#
# Arguments:
#  1. Name of function to call for each pair of elements
#  2. The first list to iterate over calling the function
#  3. The second list to iterate over calling the function
#  4. Optional argument to pass when calling the function
# Returns: The list after calling the function on each pair of elements
#
list_pairmap = \
  $(strip $(call __list_pairmap,$1,$(strip $2),$(strip $3),$(value 4)))

__list_pairmap = \
  $(if $2$3,$(call $1,$(call   list_first,$2),$(call   list_first,$3),$4) \
         $(call $0,$1,$(call list_nofirst,$2),$(call list_nofirst,$3),$4) \
   )

##
# Function: list_equal
# Compares two lists agains each other.
#
# Params:
#  1. The first list
#  2. The second list
# Returns: True if the two lists are identical
#
list_equal = \
  $(and $(filter $(words $1),$(words $2)), \
        $(findstring x $(strip $1) x,x $(strip $2) x))

##
# Function: list_reverse
# Reverses the specified list.
#
# Params:
#  1. The target list
# Returns: The list with its elements in reverse order
#
list_reverse = \
  $(strip $(call __list_reverse,$(strip $1))

__list_reverse = \
  $(if $1,$(call $0,$(call list_nofirst,$1)) $(call list_first,$1))

endif # __util_list_mk
