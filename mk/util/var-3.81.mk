#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#


__var_assign_singleline_recursive_mk = $1 = $2
__var_assign_singleline_simple_mk    = $1:= $2
__var_assign_singleline_append_mk    = $1+= $2
__var_assign_singleline_cond_mk      = $1?= $2

# eval is 3.81 bug (Savannah #27394) workaround.
${eval $( \
  )define __var_assign_multiline_recursive_mk$(\n \
  )define $$1$(\n)$$2$(\n)endef$(\n               \
  )endef$(                                        \
)}

__var_assign_multiline_mk_def = ${eval $( \
  )define __var_assign_multiline_$1_mk$(\n    \
  )$(call __var_assign_multiline_recursive_mk \
         ,__var_assign_tmp,$$2)$(\n           \
  )$(call __var_assign_singleline_$1_mk       \
         ,$$1,$$$$(__var_assign_tmp))$(\n     \
  )endef$(                                    \
)}

$(foreach op,simple append cond,$(call __var_assign_multiline_mk_def,$(op)))

MK_UTIL_VAR_NO_OPTIMIZE = 1
ifndef MK_UTIL_VAR_NO_OPTIMIZE
${eval $( \
  )define var_assign_multiline_recursive$(\n                 \
  )$${eval $(value __var_assign_multiline_recursive_mk)}$(\n \
  )endef$(                                                   \
)}
else
#var_assign_recursive = \
  ${eval $(if $(findstring $(\n),$2),,$(__var_assign_recursive_mk))}
var_assign_multiline_recursive = ${eval $(__var_assign_multiline_recursive_mk)}
endif

var_assign_multiline_simple = ${eval $(__var_assign_multiline_simple_mk)}
var_assign_multiline_append = ${eval $(__var_assign_multiline_append_mk)}
var_assign_multiline_cond   = ${eval $(__var_assign_multiline_cond_mk)}

# This is not true undefine, but
# at least "ifdef" conditionals and "+=" assignment will behave as expected.
var_assign_undefined = ${eval $1 =}

