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

#
# List manipulation functions.
#
# Author: Eldar Abusalimov
#

ifndef __util_list_mk
__util_list_mk := 1

include core/common.mk
include core/string.mk

##
# Compares two lists against each other.
#
# Params:
#  1. The first list
#  2. The second list
# Return:
#     True if the two lists are identical, false otherwise.
list_equal = \
  $(call make_bool,$(and \
        $(filter $(words $1),$(words $2)), \
        $(findstring x $(strip $1) x,x $(strip $2) x) \
   ))

##
# Reverses the specified list.
#
# Params:
#  1. The target list
# Return:
#     The list with its elements in reverse order.
list_reverse = \
  $(call list_fold,prepend,,$1)

# Higher order functions.

##
# Handler function interface for list mapping functions.
#
# Params:
#  1. An element from the list
#  2. Optional argument (if any)
# Return:
#     The value to append to the resulting list.
list_map_fn = $1

##
# Handler method interface for list mapping functions.
#
# Params:
#  1. This
#  2. An element from the list
#  3. Optional argument (if any)
# Return:
#     The value to append to the resulting list.
list_map_invoke_fn = $2

##
# Calls the specified mapping function on each element of a list.
#
# Params:
#  1. Name of function to call for each element, see 'list_map_fn'
#  2. List to iterate over
#  3. Optional argument to pass when calling the function
# Return:
#     The unstripped result of calling the function on each element.
list_map = \
  $(foreach 2,$2,$(call $1,$2,$(value 3)))

##
# Object-oriented version of 'list_map'.
#
# Params:
#  1. Name of the method to invoke for each element, see 'list_map_invoke_fn'
#  2. Object to invoke the handler on
#  3. List to iterate over
#  4. Optional argument to pass when invoking the method
# Return:
#     The unstripped result of invoking the method on each element.
list_map_invoke = \
  $(foreach 3,$3,$(call $2,$1,$3,$(value 4)))

##
# Calls the specified function on each element of a list.
# The whole list is preliminarily encoded and then each element is decoded
# separately before applying the function to it.
#
# Params:
#  1. Name of function to call for each element, see 'list_map_fn'
#  2. List to encode and iterate over decoding each element individually
#  3. (optional) Name of encoding function. Identity function by default
#  4. (optional) Name of decoding function. Identity function by default
#  5. Optional argument to pass when calling the function
# Return:
#     The unstripped result of calling the function on each transcoded element.
list_map_transcoded = \
  $(foreach 2,$(if $3,$(call $3,$2),$2) \
   ,$(call $1,$(if $4,$(call $4,$2),$2),$(value 5)))

##
# Object-oriented version of 'list_map_transcoded'.
#
# Params:
#  1. Name of the method to invoke for each element, see 'list_map_invoke_fn'
#  2. Object to invoke the handler on
#  3. List to encode and iterate over decoding each element individually
#  4. (optional) Name of encoding function. Identity function by default
#  5. (optional) Name of decoding function. Identity function by default
#  6. Optional argument to pass when invoking the method
# Return:
#     The unstripped result of invoking the method on each transcoded element.
list_map_transcoded_invoke = \
  $(foreach    3,$(if $4,$(call $4,$3),$3) \
   ,$(call $2,$1,$(if $5,$(call $5,$3),$3),$(value 6)))

##
# Handler function interface for double list mapping functions.
#
# Params:
#  1. An element from the first list
#  2. An element from the second list
#  3. Optional argument (if any)
# Return:
#     The value to append to the resulting list.
list_pairmap_fn = $1$2

##
# Handler method interface for double list mapping functions.
#
# Params:
#  1. This
#  2. An element from the first list
#  3. An element from the second list
#  4. Optional argument (if any)
# Return:
#     The value to append to the resulting list.
list_pairmap_invoke_fn = $2$3

##
# Calls the specified function on each pair of elements of two lists.
#
# Params:
#  1. Name of function to call for each pair of elements, see 'list_pairmap_fn'
#  2. The first list to iterate over
#  3. The second list to iterate over
#  4. Optional argument to pass when calling the function
# Return:
#     The unstripped result of calling the function on each pair.
list_pairmap = \
  $(call __list_pairmap,$1,$(join \
                 $(addsuffix _$$_,$(subst $$,$$$$,$2)), \
                                  $(subst $$,$$$$,$3)),$(value 4))

__list_pairmap = $(foreach 2,$2 \
  ,$(call __list_pairmap_each,$1,$(subst $$$$,$$,$(subst _$$_, _ ,$2)),$3))
__list_pairmap_each = $(if $(call singleword,$2) \
    ,$(call $1,,$2,$3),$(call $1,$(word 1,$2),$(word 3,$2),$3))

##
# Object-oriented version of 'list_pairmap'.
#
# Params:
#  1. Name of method to invoke for each pair of elements, see
#    'list_pairmap_invoke_fn'
#  2. Object to invoke the handler on
#  3. The first list to iterate over
#  4. The second list to iterate over
#  5. Optional argument to pass when invoking the method
# Return:
#     The unstripped result of invoking the method on each pair.
list_pairmap_invoke = \
  $(call __list_pairmap_invoke,$2,$1,$(join \
                 $(addsuffix _$$_,$(subst $$,$$$$,$3)), \
                                  $(subst $$,$$$$,$4)),$(value 5))

__list_pairmap_invoke = $(foreach 3,$3,$(call __list_pairmap_each_invoke \
    ,$1,$2,$(subst $$$$,$$,$(subst _$$_, _ ,$3)),$4))
__list_pairmap_each_invoke = $(if $(call singleword,$3) \
    ,$(call $1,$2,,$3,$4),$(call $1,$2,$(word 1,$3),$(word 3,$3),$4))

# Left folding functions.

#
# Implementation note:
#
# Using imperative version (which clobbers context during iterations using
# 'eval') intead of pure functional style implementation (using recursion) is
# much faster (and gives O(n) complexity as seen from GNU Make sources).
# It is about 1500 times faster on folding list of 10000 elements using
# identity function as combining.
#
# The fast implementation is enabled by default. To get back and use pure
# functions please define LIST_PURE_FUNC macro.
#
ifndef LIST_PURE_FUNC
__list_fold__ :=
endif

##
# Combining function interface for left folding functions.
# Functions which are passed to fold/scan and their derivatives should follow
# semantics defined by the interface.
#
# Params:
#  1. Intermediate value obtained as the result of previous function calls
#  2. An element from the list being folded
#  3. Optional argument (if any)
# Return:
#     The value to pass to the next function call as new intermediate value, if
#     there are more elements in the list.
#     Otherwise this value is used as the return value of fold/scan.
list_fold_fn = $1

##
# Combining method interface for left folding functions.
#
# Params:
#  1. This
#  2. Intermediate value obtained as the result of previous method invocations
#  3. An element from the list being folded
#  4. Optional argument (if any)
# Return:
#     The value to pass to the next method invocation as new intermediate
#     value, if there are more elements in the list.
#     Otherwise this value is used as the return value of fold/scan.
list_fold_invoke_fn = $1

##
# Takes the second argument and the first item of the list and applies the
# function to them, then feeds the function with this result and the second
# argument and so on.
#
# Params:
#  1. Name of the combining function, see 'list_fold_fn'
#  2. Initial value to pass as an intermediate value when calling function
#     for the first time
#  3. List to iterate over applying the folding function
#  4. Optional argument to pass when calling the function
# Return:
#     The result of the last function call (if any occurred),
#     or the initial value in case of empty list.
# See: list_scan
#     which preserves intermediate results
list_fold = \
  $(call __list_fold,$1,$2,$3,$(value 4))

ifndef LIST_PURE_FUNC
__list_fold = \
  $(and ${eval __list_fold__ := \
            $$2}$(foreach 3,$3,${eval __list_fold__ := \
                  $$(call $$1,$$(__list_fold__),$$3,$$4)}),)$(__list_fold__)

else
__list_fold = \
  $(call __list_fold_stripped,$1,$2,$(strip $3),$4)
__list_fold_stripped = \
  $(if $3,$(call $0 \
      ,$1,$(call $1,$2,$(call firstword,$3),$4),$(call nofirstword,$3),$4),$2)

endif

##
# Object-oriented version of 'list_fold'.
#
# Params:
#  1. Name of the combining method, see 'list_fold_invoke_fn'
#  2. Object to invoke the handler on
#  3. Initial value to pass as an intermediate value when calling function
#     for the first time
#  4. List to iterate over applying the folding function
#  5. Optional argument to pass when calling the function
# Return:
#     The result of the last function call (if any occurred),
#         or the initial value in case of empty list.
# See: list_scan_invoke
#     which preserves intermediate results
list_fold_invoke = \
  $(call __list_fold_invoke,$2,$1,$3,$4,$(value 5))

ifndef LIST_PURE_FUNC
__list_fold_invoke = \
  $(and ${eval __list_fold__ := \
            $$3}$(foreach 4,$4,${eval __list_fold__ := \
                 $$(call $$1,$$2,$$(__list_fold__),$$4,$$5)}),)$(__list_fold__)

else
__list_fold_invoke = \
  $(call __list_fold_stripped_invoke,$1,$2,$3,$(strip $4),$5)
__list_fold_stripped_invoke = \
  $(if $4,$(call $0 \
      ,$1,$2,$(call $1,$2,$3,$(call firstword,$4),$5),$(call nofirstword,$4),$5),$3)

endif

##
# Takes the second argument and the first item of the list and applies the
# function to them, then feeds the function with this result and the second
# argument and so on.
#
# Params:
#     See 'list_fold'.
# Return:
#     The list of intermediate and final results of the function calls (if any
#     occurred),
#     or the initial value in case of empty list.
list_scan = \
  $(call __list_scan,$1,$2,$3,$(value 4))

ifndef LIST_PURE_FUNC
__list_scan = \
  ${eval __list_fold__ := \
        $$2}$(foreach 3,$3,$(__list_fold__)${eval __list_fold__ := \
              $$(call $$1,$$(__list_fold__),$$3,$$4)})$(if $(firstword $3), \
   )$(__list_fold__)

else
__list_scan = \
  $(call __list_scan_stripped,$1,$2,$(strip $3),$4)
__list_scan_stripped = \
  $2$(if $3, $(call $0 \
        ,$1,$(call $1,$2,$(call firstword,$3),$4),$(call nofirstword,$3),$4))

endif

##
# Object-oriented version of 'list_scan'.
#
# Params:
#     See 'list_fold_invoke'.
# Return:
#     The list of intermediate and final results of the method invocations (if
#     any occurred),
#     or the initial value in case of empty list.
list_scan_invoke = \
  $(call __list_scan_invoke,$2,$1,$3,$4,$(value 5))

ifndef LIST_PURE_FUNC
__list_scan_invoke = \
  ${eval __list_fold__ := \
        $$3}$(foreach 4,$4,$(__list_fold__)${eval __list_fold__ := \
            $$(call $$1,$$2,$$(__list_fold__),$$4,$$5)})$(if $(firstword $4), \
   )$(__list_fold__)

else
__list_scan_invoke = \
  $(call __list_scan_stripped_invoke,$1,$2,$3,$(strip $4),$5)
__list_scan_stripped_invoke = \
  $3$(if $4, $(call $0,$1,$2 \
        ,$(call $1,$2,$3,$(call firstword,$4),$5),$(call nofirstword,$4),$5))

endif

endif # __util_list_mk
