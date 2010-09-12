#
# Variable assignment functions for GNU Make 3.81
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

# This is not true undefine, but
# at least "ifdef" conditionals and "+=" assignment will behave as expected.
__var_assign_undefined_mk = $1 =

__var_assign_inline := \
  $(filter-out $(addprefix multiline_,simple append cond), \
     $(__var_assign_lines_ops))

