#
#   Date: Nov 20, 2010
# Author: Eldar Abusalimov
#

ifndef __util_var_name_mk
__util_var_name_mk := 1

include util/common.mk
include util/math.mk
include util/list.mk

# TODO only single words and whitespace separated pairs for now... -- Eldar
var_name_escape = $(call __var_name_escape1,$(or $(value 1),$(.VARIABLES)))
var_name_unescape = $(subst $$$$,$$,$(call __var_name_unescape_whitespace,$1))

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
    $(if $(findstring undefined,$(flavor \
                $(call var_name_unescape,$(combo)))),,$(combo))$ \
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
  $(foreach single,$1,$(if $(or $(findstring undefined,$(flavor $(single))),$ \
            $(filter-out 1,$(words $(filter $(single),$1)))),,$(single)))

# Params:
#  1. Unescaped variables list
#  2. Remaining singles
# Returns: the variable list with all singles removed (not escaped)
__var_name_multies = \
  $(if $2,$(call $0,$(subst $( ) $(call first,$2) , ,$1),$(call rest,$2)),$1)

__var_name_escape_whitespace = \
  $(subst $(\space),_$$s,$(subst $(\t),_$$t,$(subst $(\n),_$$n,$1)))
__var_name_unescape_whitespace = \
  $(subst _$$s,$(\space),$(subst _$$t,$(\t),$(subst _$$n,$(\n),$1)))

endif # __util_var_name_mk
