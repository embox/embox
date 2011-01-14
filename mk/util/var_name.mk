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
# Variable identifiers mangling/demangling.
#
#   Date: Nov 20, 2010
# Author: Eldar Abusalimov
#

ifndef __util_var_name_mk
__util_var_name_mk := 1

#
# Usage example:
#
##
#  include util/var_name.mk
#
#  # First, define two variables (one of them has whitespaces in its name):
#     a_regular_single_word_named_variable = foo
#     a one with whitespaces   in its name = bar
#
#  # A list containing their names looks like this:
#  variables_list = \
#     a_regular_single_word_named_variable \
#     a one with whitespaces   in its name
#
#  # Make sees 8 whitespace separated words in variables_list:
#  $(info variables_list has $(words $(variables_list)) words in it)
#
#  # Mangle the list!
#  mangled_variables_list := \
#     $(call var_name_mangle,$(variables_list) and some undefined garbage)
#
#  # The mangled list contains only 2 words:
#  $(info mangled_variables_list contains only \
#          $(words $(mangled_variables_list)) words)
#
#  # ... and we can safely iterate over them
#  #     demangling each element to restore the initial value:
#  $(foreach var,$(mangled_variables_list), \
#      $(info var name found: \
#                    [$(call var_name_demangle,$(var))]) \
#   )
#
#  # The output of the code above will be:
#  # variables_list has 8 words in it
#  # mangled_variables_list contains only 2 words
#  # var name found: [a_regular_single_word_named_variable]
#  # var name found: [and   with whitespaces   in its name]
#

include util/common.mk
include util/var_info.mk # var_defined
include util/math.mk # sequences generator
include util/list.mk # first/rest

#
# Function: var_name_mangle
#
# Escapes variables list in a special way so that it becomes possible to
# iterate over it using foreach, even if some variables contain whitespaces in
# their names.
# The name of each variable as is can be restored back using
# 'var_name_demangle' function.
#
# Params:
#  1. (optional) Variables list.
#     If not specified, the value of .VARIABLES built-in variable is used
# Returns: whitespace separated list, each element of which is mangled name of
#          a defined variable from the specified list (or from $(.VARIABLES))
#
var_name_mangle = $(call __var_name_escape1,$(or $(value 1),$(.VARIABLES)))

#
# Function: var_name_demangle
#
# Unescapes the result of 'var_name_mangle' call.
#
# Params:
#  1. Mangled variable name (or list of mangled names, although then
#                             mangling/demangling makes no sense)
# Returns: Variable name as it has to be before the mangling
#
var_name_demangle = $(subst $$$$,$$,$(call __var_name_unescape_whitespace,$1))

# Params:
#  1. Variables list
__var_name_escape1 = \
  $(call __var_name_escape2,$1,$(call __var_name_singles,$1))

# Params:
#  1. Variables list
#  2. Singleword-named variables
__var_name_escape2 = \
  $(call __var_name_escape3,$1,$2,$(call __var_name_multies,$1,$2))

# Params:
#  1. Variables list
#  2. Singleword-named variables
#  3. Multiword-named ones
__var_name_escape3 = $(subst $$,$$$$,$2) \
  $(foreach combo,$(call __var_name_escape_combos,$ \
                $(subst $$,$$$$,$3),$(call int_seq,x,$(3:%=x)),$(words $3)),$ \
    $(if $(call var_undefined,$(call var_name_demangle,$(combo))),,$(combo))$ \
   )

# Params:
#  1. Dollar-escaped list
#  2. Sequence
#  3. Length
__var_name_escape_combos = \
  $(foreach start,$(call rest,$2),$(call __var_name_escape_do_combo, \
    $1,$2,$(wordlist $(start),$3,$2)))

# Params:
#  1. Dollar-escaped list
#  2. Sequence
#  3. Tail of the sequence
__var_name_escape_do_combo = \
  $(call pairmap,__var_name_escape_do_combo_pairmap, \
     $(wordlist 1,$(words $3),$2),$3,$1)

# Params:
#  1. Start index
#  2. End index
#  3. The list
__var_name_escape_do_combo_pairmap = \
  $(call __var_name_escape_whitespace,$(wordlist $1,$2,$3))

# Params:
#  1. Unescaped variables list
# Returns: list of singleword-named variables (still not escaped)
__var_name_singles = \
  $(foreach single,$1,$(if $(and \
        $(call var_defined,$(single)), \
        $(call list_single,$(filter $(single),$1))),$(single)))

# Params:
#  1. Unescaped variables list
#  2. Remaining singles
# Returns: the variable list with all singles removed (not escaped)
__var_name_multies = \
  $(if $2,$(call $0,$(subst $( ) $(call first,$2) , , $1 ),$(call rest,$2)),$1)

# Escape/unescape space, tab and new line.

__var_name_escape_whitespace = \
  $(subst $(\space),_$$s,$(subst $(\t),_$$t,$(subst $(\n),_$$n,$1)))
__var_name_unescape_whitespace = \
  $(subst _$$s,$(\space),$(subst _$$t,$(\t),$(subst _$$n,$(\n),$1)))

# TODO

endif # __util_var_name_mk
