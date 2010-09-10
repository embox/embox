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

__var_make_version := $(if $(call make_version_gte,3.82),3.82,$(MAKE_VERSION))
include $(self_makefile_dir)/var-$(__var_make_version).mk

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

