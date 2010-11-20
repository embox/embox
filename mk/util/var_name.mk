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
var_name_escape = $(call __var_name_escape,$(or $(value 1),$(.VARIABLES)))
var_name_unescape = $(subst $$$$,$$,$(call __var_name_unescape_whitespace,$1))

__var_name_escape = \
  $(foreach combo,$(call __var_name_escape_combos,$ \
                        $(subst $$,$$$$,$1),$(call int_seq,x,$(1:%=x))),$ \
    $(if $(findstring undefined,$(flavor \
                $(call var_name_unescape,$(combo)))),,$(combo))$ \
   )

__var_name_escape_combos = \
  $1 $(call __var_name_escape_pairs,$1,$2)# TODO

__var_name_escape_pairs = \
  $(call pairmap,__var_name_escape_pairs_pairmap,$$ $2,$2 $$,$1)

# | iteration |   <1>   |   <2>   |   <3>   |   <4>  |
# +-----------+---------+---------+---------+--------+
# |  arg 1    |    $$   |    1    |    2    |    3   |
# |  arg 2    |    1    |    2    |    3    |    $$  |
# | result    |         | foo-bar | bar-baz |        |
__var_name_escape_combo_pairmap = $(call __var_name_escape_whitespace,$ \
  $(if $(findstring $$,$1$2),,$(wordlist $1,$2,$3))$ \
)

__var_name_escape_combo_XXX = \
  $(call triplemap,__var_name_escape_combo_triplemap,$$ $2,$2 $$,$1)

# | iteration |    <1>    |    <2>    |    <3>   |
# +-----------+-----------+-----------+----------+
# |  arg 1    |    $$     |foo-bar    |bar-baz   |
# |  arg 2    |    foo-bar|    bar-baz|    $$    |
# |  arg 3    |    foo    |    bar    |    baz   |
# | result    |           |foo-bar-baz|          |
__var_name_escape_combo_triplemap = \
  $(if $(findstring $$,$1$2),,$(subst $3 ,,$1 )$2)

__var_name_escape_whitespace = \
  $(subst $(\space),_$$s,$(subst $(\t),_$$t,$(subst $(\n),_$$n,$1)))
__var_name_unescape_whitespace = \
  $(subst _$$s,$(\space),$(subst _$$t,$(\t),$(subst _$$n,$(\n),$1)))

endif # __util_var_name_mk
