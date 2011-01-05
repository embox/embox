#
# Author: Eldar Abusalimov
#

ifndef __util_object_mk
__util_object_mk := 1

# 1. clazz
# ... args
# ret: id
object_new =
# 1. obj
# 2. clazz
# ret: true/false
object_instanceof =
# 1. obj
object_delete =

__object_instance_counter :=
__object_instance_counter_get = $(words $(__object_instance_counter))
__object_instance_counter_get_preincrement = \
  ${eval __object_instance_counter += x}$(__object_instance_counter_get)

endif # __util_object_mk

