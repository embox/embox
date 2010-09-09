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

var_define   = \
  $(call assert_called,var_define,$0)$(call var_assign_recursive,$1,$2)
var_undefine = \
  $(call assert_called,var_undefine,$0)$(call var_assign_undefined,$1,$2)

var_swap = $(strip \
  $(call assert_called,var_swap,$0) \
  $(call __var_swap,$1,$2,__var_swap_tmp) \
)
__var_swap = \
  $(call var_assign_$(flavor $1),$3,$(value $1)) \
  $(call var_assign_$(flavor $2),$1,$(value $2)) \
  $(call var_assign_$(flavor $3),$2,$(value $3))

var_save = $(strip \
  $(call assert_called,var_save,$0) \
  $(call set,__var_value,$1,$(value $1)) \
  $(call set,__var_flavor,$1,$(flavor $1)) \
)
var_restore = $(strip \
  $(call assert_called,var_restore,$0) \
  $(call var_assign_$(call get,__var_flavor,$1),$1,$(call get,__var_value,$1))\
)

