#
# Variable assignment functions optimized for GNU Make 3.82
#
# Author: Eldar Abusalimov
#

__var_assign_singleline_recursive_mk = $1 = $2
__var_assign_singleline_simple_mk    = $1:= $2
__var_assign_singleline_append_mk    = $1+= $2
__var_assign_singleline_cond_mk      = $1?= $2

__var_assign_mk_def = ${eval $( \
  )define $1$(\n     \
  )define $$1 $2$(\n \
  )$$2$(\n           \
  )endef$(\n         \
  )endef$(           \
)}

$(call __var_assign_mk_def,__var_assign_multiline_recursive_mk,=)
$(call __var_assign_mk_def,__var_assign_multiline_simple_mk,:=)
$(call __var_assign_mk_def,__var_assign_multiline_append_mk,+=)
$(call __var_assign_mk_def,__var_assign_multiline_cond_mk,?=)

# Since version 3.82 GNU Make provides native 'undefine' directive.
__var_assign_undefined_mk = undefine $1

# Inline everything.
__var_assign_inline := $(__var_assign_lines_ops)
