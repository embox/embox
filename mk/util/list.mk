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

# Some parts of this file are derived from GMSL.

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

include core/string.mk

##
# Function: list_map
# Calls the specified function on each element of a list.
#
# Params:
#  1. Name of function to call for each element,
#     with the following signature:
#       1. An element from the list
#       2. Optional argument (if any)
#      Return: the value to append to the resulting list
#  2. List to iterate over
#  3. Optional argument to pass when calling the function
# Return: The list after calling the function on each element
#
list_map = \
  $(strip $(foreach 2,$2,$(call $1,$2,$(value 3))))
# TODO $(foreach 2,$2,...) can be a bitch, check it. -- Eldar

##
# Function: list_pairmap
# Calls the specified function on each pair of elements of two lists.
#
# Params:
#  1. Name of function to call for each pair of elements,
#     with the following signature:
#       1. An element from the first list
#       2. An element from the second list
#       3. Optional argument (if any)
#      Return: the value to append to the resulting list
#  2. The first list to iterate over
#  3. The second list to iterate over
#  4. Optional argument to pass when calling the function
# Return: The list after calling the function on each pair of elements
#
list_pairmap = \
  $(strip $(call __list_pairmap,$1,$(strip $2),$(strip $3),$(value 4)))

__list_pairmap = \
  $(if $2$3,$(call $1,$(call   firstword,$2),$(call   firstword,$3),$4) \
         $(call $0,$1,$(call nofirstword,$2),$(call nofirstword,$3),$4) \
   )

##
# Function: list_foldl
# Takes the second argument and the first item of the list and applies the
# function to them, then feeds the function with this result and the second
# argument and so on.
#
# Params:
#  1. Name of the folding function,
#     with the following signature:
#       1. Intermediate value obtained as the result of previous function calls
#       2. An element from the list
#       3. Optional argument (if any)
#      Return: the value to pass to the next function call
#  2. Initial value to pass as an intermediate value when calling function
#     for the first time
#  3. List to iterate over applying the folding function
#  4. Optional argument to pass when calling the function
# Return: The result of the last function call (if any occurred),
#         or the initial value in case of empty list
#
list_foldl = \
  $(call __list_foldl,$1,$2,$(strip $3),$(value 4))

__list_foldl = \
  $(if $3,$ \
    $(call $0,$1,$ \
            $(call $1,$2,$(call firstword,$3),$4),$ \
        $(call nofirstword,$3),$4),$ \
    $2)

list_scanl = \
  $(call __list_scanl,$1,$2,$(strip $3),$(value 4))

__list_scanl = \
  $2$(if $3, \
      $(call $0,$1,$ \
              $(call $1,$2,$(call firstword,$3),$4),$ \
          $(call nofirstword,$3),$4)$ \
      )

##
# Function: list_foldr
# Takes the last item of the list and the second argument and applies the
# function, then it takes the penultimate item from the end and the result,
# and so on.
#
# Params:
#  1. Name of the folding function,
#     with the following signature:
#       1. An element from the list
#       2. Intermediate value obtained as the result of previous function calls
#       3. Optional argument (if any)
#      Return: the value to pass to the next function call
#  2. Initial value to pass as an intermediate value when calling function
#     for the first time
#  3. List to iterate over applying the folding function
#  4. Optional argument to pass when calling the function
# Return: The result of the last function call (if any occurred),
#         or the initial value in case of empty list
#
list_foldr = \
  $(call __list_foldr,$1,$2,$(strip $3),$(value 4))

__list_foldr = \
  $(if $3,$ \
    $(call $1,$(call firstword,$3),$ \
            $(call $0,$1,$2,$(call nofirstword,$3),$4),$ \
        $4),$ \
    $2)

list_scanr = \
  $(call __list_scanr,$1,$2,$(strip $3),$(value 4))

__list_scanr = \
  $(if $3,$ \
      $(call $0,$1,$ \
              $(call $1,$(call lastword,$3),$2,$4),$ \
          $(call nolastword,$3),$4) \
      )$2

##
# Function: list_foldl1
# Takes the first two items of the list and applies the function to them, then
# feeds the function with this result and the third argument and so on.
#
# Params:
#  1. Name of the folding function,
#     with the following signature:
#       1. Intermediate value obtained as the result of previous function calls
#       2. An element from the list
#       3. Optional argument (if any)
#      Return: the value to pass to the next function call
#  2. List to iterate over applying the folding function
#  3. Optional argument to pass when calling the function
# Return: The result of the last function call (if any occurred),
#         the list element if it is the only element in the list,
#         or empty if the list is empty
#
list_foldl1 = \
  $(if $(strip $2),$ \
      $(call __list_foldl,$1,$(call firstword,$2),$ \
                           $(call nofirstword,$2),$(value 3)))

##
# Function: list_foldr1
# Takes the last two items of the list and applies the function, then it takes
# the third item from the end and the result, and so on.
#
# Params:
#  1. Name of the folding function,
#     with the following signature:
#       1. An element from the list
#       2. Intermediate value obtained as the result of previous function calls
#       3. Optional argument (if any)
#      Return: the value to pass to the next function call
#  2. List to iterate over applying the folding function
#  3. Optional argument to pass when calling the function
# Return: The result of the last function call (if any occurred),
#         the list element if it is the only element in the list,
#         or empty if the list is empty
#
list_foldr1 = \
  $(if $(strip $2),$ \
      $(call __list_foldr,$1,$(call lastword,$2),$ \
                           $(call nolastword,$2),$(value 3)))

##
# Function: list_equal
# Compares two lists against each other.
#
# Params:
#  1. The first list
#  2. The second list
# Return: True if the two lists are identical, false otherwise
#
list_equal = \
  $(call make_bool,$(and \
        $(filter $(words $1),$(words $2)), \
        $(findstring x $(strip $1) x,x $(strip $2) x) \
   ))

##
# Function: list_reverse
# Reverses the specified list.
#
# Params:
#  1. The target list
# Return: The list with its elements in reverse order
#
list_reverse = \
  $(strip $(call __list_reverse,$(strip $1)))

__list_reverse = \
  $(if $1,$(call $0,$(call nofirstword,$1)) $(call firstword,$1))

endif # __util_list_mk
