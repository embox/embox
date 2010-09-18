#
# Variable assignment functions for GNU Make 3.81
#
# Author: Eldar Abusalimov
#

__var_assign_singleline_mk_def = ${eval $1 = $$1 $2 $$2}

# eval is 3.81 bug (Savannah #27394) workaround.
${eval $ \
  define __var_assign_multiline_recursive_mk$(\n)$ \
  define $$1$(\n)$$2$(\n)endef$(\n)$               \
  endef$                                           \
}

__var_assign_multiline_mk_def = ${eval $ \
  define __var_assign_multiline_$1_mk$(\n)$  \
  $(call __var_assign_multiline_recursive_mk \
         ,__var_assign_tmp,$$2)$(\n)$        \
  $(call __var_assign_singleline_$1_mk       \
         ,$$1,$$$$(__var_assign_tmp))$(\n)$  \
  endef$                                     \
}

$(foreach op,$(__var_assign_ops), \
  $(call __var_assign_singleline_mk_def \
        ,__var_assign_singleline_$(op)_mk,$(__var_assign_op_$(op))) \
)

$(call __var_assign_multiline_mk_def,simple)

# XXX
__var_assign_multiline_append_mk = $(error $0: not yet implemented)
__var_assign_multiline_cond_mk = $(error $0: not yet implemented)

# This is not true undefine, but
# at least "ifdef" conditionals and "+=" assignment will behave as expected.
__var_assign_undefined_mk = $1 =

__var_assign_inline := \
  $(filter-out $(addprefix multiline_,simple append cond), \
     $(__var_assign_lines_ops))

