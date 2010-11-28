#
#   Date: Sep 10, 2010
# Author: Eldar Abusalimov
#


check_attach = $(strip $(if $(value $(__unchecked_name)), \
  $(call var_assign_append,$(__check_hooks),$2), \
  $(__check_attach_init) \
)

__check_attach_init = \
  $(call var_assign_$(flavor $1),$(__unchecked_name),$(value $1)) \
  $(call var_assign_$(flavor $1),$1,$(__check_invoke)$$($(__unchecked_name))) \
  $(call var_assign_simple,$(__check_hooks),)

__check_invoke = $$(foreach check_target,$1 \
  ,$$(foreach __check_hook,$$($(__check_hooks)),$$(__check_hook)))

check_hook_assert_called = $(call assert_called,$(check_target),$0)

__unchecked_name = __unchecked_$1
__check_hooks = __checks_$1

