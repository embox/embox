#
# Variable assignment functions optimized for GNU Make 3.82
#
# Author: Eldar Abusalimov
#

__var_assign_mk_def = ${eval $( \
  )define $1$(\n     \
  )define $$1 $2$(\n \
  )$$2$(\n           \
  )endef$(\n         \
  )endef$(           \
)}

$(call __var_assign_mk_def,__var_assign_recursive_mk,=)
$(call __var_assign_mk_def,__var_assign_simple_mk,:=)
$(call __var_assign_mk_def,__var_assign_append_mk,+=)
$(call __var_assign_mk_def,__var_assign_cond_mk,?=)

ifndef MK_UTIL_VAR_NO_OPTIMIZE
__var_assign_def = ${eval $( \
  )define $1$(\n             \
  )$${eval $(value $2)}$(\n  \
  )endef$(                   \
)}
else
__var_assign_def = ${eval $1 = $${eval $$($2)}}
endif

$(call __var_assign_def,var_assign_recursive,__var_assign_recursive_mk)
$(call __var_assign_def,var_assign_simple,__var_assign_simple_mk)
$(call __var_assign_def,var_assign_append,__var_assign_append_mk)
$(call __var_assign_def,var_assign_cond,__var_assign_cond_mk)

# Since version 3.82 GNU Make provides native 'undefine' directive.
var_assign_undefined = ${eval undefine $1}

