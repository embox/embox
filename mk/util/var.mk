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

endef

__var_assign_lines := singleline multiline
__var_assign_ops   := recursive simple append cond

__var_assign_lines_ops := $(sort undefined \
  $(foreach line,$(__var_assign_lines), \
    $(foreach op,$(__var_assign_ops), \
      $(line)_$(op) \
    ) \
  ) \
)

# Include version-specific file.
#
# It have to define __var_assign_<line>_<op>_mk variables (8 ones total),
# where <line> is: singleline|multiline,
#     and <op> is: recursive|simple|append|cond,
# and __var_assign_undefined_mk variable.
#
# __var_assign_inline list should be populated with <line>_<op> values
# for which var_assign_<line>_<op> variables could be inlined when being
# constructed from the corresponding __var_assign_<line>_<op>_mk ones.
__var_assign_inline :=

# So, include it.
__var_make_version := $(if $(call make_version_gte,3.82),3.82,3.81)
include $(self_makefile_dir)var-$(__var_make_version).mk

__var_assign_mk_missing := $(strip \
  $(foreach def,$(__var_assign_lines_ops:%=__var_assign_%_mk), \
    $(if $(filter undefined,$(origin $(def))),$(def)) \
  ) \
)
$(call assert,$(call not,$(__var_assign_mk_missing)),Missing def: \
  $(__var_assign_mk_missing). Check you implementation)

__var_assign_inline_bogus := \
  $(filter-out $(__var_assign_lines_ops),$(__var_assign_inline))
$(call assert,$(call not,$(__var_assign_inline_bogus)),Invalid values \
  came from var-$(__var_make_version).mk: $(__var_assign_inline_bogus). \
  Check you implementation)

__var_assign_lines_ops_inline := \
  $(sort $(filter $(__var_assign_lines_ops),$(__var_assign_inline)))

__var_assign_lines_ops_noinline := \
  $(sort $(filter-out $(__var_assign_inline),$(__var_assign_lines_ops)))

ifndef MK_UTIL_VAR_NO_INLINE
__var_assign_def_inline = ${eval $( \
  )define $1$(\n             \
  )$${eval $(value $2)}$(\n  \
  )endef$(                   \
)}
else
__var_assign_def_inline = __var_assign_def_noinline
endif
__var_assign_def_noinline = ${eval $1 = $${eval $$($2)}}

$(foreach xline,inline noinline, \
  $(foreach def,$(__var_assign_lines_ops_$(xline):%=var_assign_%), \
    $(call __var_assign_def_$(xline),$(def),__$(def)_mk) \
  ) \
)

__var_assign_autoline_def = ${eval $( \
  )var_assign_$1 = $${eval $$(if $$(findstring $$(\n),$$2),$( \
    )$$(__var_assign_multiline_$1_mk),$( \
    )$$(__var_assign_singleline_$1_mk)$( \
  ))}$( \
)}

$(foreach op,$(__var_assign_ops), \
  $(call __var_assign_autoline_def,$(op)) \
)

# Yes, everything above may look horrible, but it is only at first sight.
# If you are trying to understand how does it work,
# just enable the following block.
# -- Eldar
ifeq (1,0)
__var_info_simplified = $(info $1 = $(\n)$(value $1)$(\n))
$(call __var_info_simplified,var_assign_undefined)
$(call __var_info_simplified,var_assign_recursive)
$(call __var_info_simplified,var_assign_simple)
$(call __var_info_simplified,var_assign_append)
$(call __var_info_simplified,var_assign_cond)
$(call __var_info_simplified,var_assign_singleline_recursive)
$(call __var_info_simplified,var_assign_singleline_simple)
$(call __var_info_simplified,var_assign_singleline_append)
$(call __var_info_simplified,var_assign_singleline_cond)
$(call __var_info_simplified,var_assign_multiline_recursive)
$(call __var_info_simplified,var_assign_multiline_simple)
$(call __var_info_simplified,var_assign_multiline_append)
$(call __var_info_simplified,var_assign_multiline_cond)
$(call __var_info_simplified,__var_assign_singleline_recursive_mk)
$(call __var_info_simplified,__var_assign_singleline_simple_mk)
$(call __var_info_simplified,__var_assign_singleline_append_mk)
$(call __var_info_simplified,__var_assign_singleline_cond_mk)
$(call __var_info_simplified,__var_assign_multiline_recursive_mk)
$(call __var_info_simplified,__var_assign_multiline_simple_mk)
$(call __var_info_simplified,__var_assign_multiline_append_mk)
$(call __var_info_simplified,__var_assign_multiline_cond_mk)
endif

# XXX rewrite everything below. -- Eldar

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

