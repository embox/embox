#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#


var_info = $(call assert_called,var_info,$0)$(info $(call var_dump,$1))

define var_dump
$(call assert_called,var_dump,$0) \
  $(call assert,$1,Variable name is empty) Variable [$1] info:
   flavor: [$(flavor $1)]
   origin: [$(origin $1)]
    value: [$(value $1)]
expansion: [$($1)]

endef

# eval is 3.81 bug (Savannah #27394) workaround.
${eval $( \
  )$(\n)define __var_define_mk$(       \
  )$(\n)define $$1$(\n)$$2$(\n)endef$( \
  )$(\n)endef$(                        \
)}

${eval $( \
  )$(\n)define __var_assign_recursive_mk$( \
  )$(\n)$(call __var_define_mk,$$1,$$2)$(  \
  )$(\n)endef$(                            \
)}

__var_assign_template_mk_def = ${eval $(                \
  )$(\n)define $1$(                                     \
  )$(\n)$(call __var_define_mk,__var_assign_tmp,$$2)$(  \
  )$(\n)$$1 $2 $$$$(__var_assign_tmp)$(                 \
  )$(\n)endef$(                                         \
)}

$(call __var_assign_template_mk_def,__var_assign_simple_mk,:=)
$(call __var_assign_template_mk_def,__var_assign_append_mk,+=)
$(call __var_assign_template_mk_def,__var_assign_cond_mk,?=)

ifndef MK_UTIL_VAR_NO_OPTIMIZE
${eval $(                                             \
  )$(\n)define var_assign_recursive$(                 \
  )$(\n)$${eval $(value __var_assign_recursive_mk)}$( \
  )$(\n)endef$(                                       \
)}
else
var_assign_recursive = ${eval $(__var_assign_recursive_mk)}
endif

var_assign_simple = ${eval $(__var_assign_simple_mk)}
var_assign_append = ${eval $(__var_assign_append_mk)}
var_assign_cond   = ${eval $(__var_assign_cond_mk)}

ifeq ($(MAKE_VERSION),3.81)
var_assign_undefined = $(strip \
  $(call assert_called,var_assign_undefined,$0) \
  $(if $(filter-out undefined,$(origin $1)),$(call __var_assign,$1:=,)) \
)
else # Since version 3.82 GNU Make provides true 'undefine' directive.
var_assign_undefined = \
  $(call assert_called,var_assign_undefined,$0)$(call __var_assign,undefine $1,)
endif

