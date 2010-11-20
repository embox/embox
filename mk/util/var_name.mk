#
#   Date: Nov 20, 2010
# Author: Eldar Abusalimov
#

ifndef __util_var_name_mk
__util_var_name_mk := 1

include util/common.mk
include util/math.mk
include util/list.mk

# TODO only whitespace separated pairs for now... -- Eldar
var_name_escape = $(call __var_name_escape,$(or $(value 1),$(.VARIABLES)))
var_name_unescape = $(subst $$$$,$$,$(call __var_name_unescape_whitespace,$1))

__var_name_escape = \
  $(foreach combo,$(call __var_name_escape_combos,$ \
                        $(subst $$,$$$$,$1),$(call int_seq,x,$(1:%=x))),$ \
    $(if $(findstring undefined,$(flavor \
                $(call var_name_unescape,$(combo)))),,$(combo))$ \
   )

__var_name_escape_combos = \
  $(call __var_name_escape_combo_pairs,$1,$2)# TODO

__var_name_escape_combo_pairs = \
  $(call pairmap,__var_name_escape_combo_pairs_pairmap,x $2,$2 x,$1)

# 1. i
# 2. i+1
# 3. variables
__var_name_escape_combo_pairs_pairmap = $(call __var_name_escape_whitespace,$ \
  $(if $(findstring x,$1),$(firstword $3),$ \
       $(if $(findstring x,$2),$(lastword $3),$ \
            $(wordlist $1,$2,$3)$ \
        )$ \
   )$ \
)

__var_name_escape_whitespace = \
  $(subst $(\space),_$$s,$(subst $(\t),_$$t,$(subst $(\n),_$$n,$1)))
__var_name_unescape_whitespace = \
  $(subst _$$s,$(\space),$(subst _$$t,$(\t),$(subst _$$n,$(\n),$1)))

endif # __util_var_name_mk
