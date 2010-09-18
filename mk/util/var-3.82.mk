#
# Variable assignment functions optimized for GNU Make 3.82
#
# Author: Eldar Abusalimov
#

__var_assign_singleline_mk_def = \
  ${eval $1 = $$1 $2 $$2}

__var_assign_multiline_mk_def = \
  ${eval $(              \
    )define $1$(\n       \
      )define $$1 $2$(\n \
        )$$2$(\n         \
      )endef$(\n         \
    )endef$(             \
  )}

$(foreach line,$(__var_assign_lines), \
  $(foreach op,$(__var_assign_ops), \
    $(call __var_assign_$(line)_mk_def \
          ,__var_assign_$(line)_$(op)_mk,$(__var_assign_op_$(op))) \
  ) \
)

# Since version 3.82 GNU Make provides native 'undefine' directive.
__var_assign_undefined_mk = undefine $1

# Inline everything.
__var_assign_inline := $(__var_assign_lines_ops)
