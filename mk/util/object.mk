#
#   Date: Dec 20, 2010
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

# Params:
#  1. Class name
object_class_define =
# Params:
#  1. Class name
#  2. Field name
#  3. Optional getter
#  4. Optional setter
object_class_define_field =
# Params:
#  1. Class name
#  2. Method name
#  3. Function to bind
object_class_define_method =

# Params:
#  1. Instance on which to invoke the method
#  2. Method to invoke
# ... Args
invoke =

# Params:
#  1. Instance which's field to get
#  2. The field
# Returns: The field value
get =

# Params:
#  1. Instance which's field to set
#  2. The field
#  3. The value to set
set =

__object_instance_counter :=
__object_instance_counter_get = $(words $(__object_instance_counter))
__object_instance_counter_get_preincrement = \
  ${eval __object_instance_counter += x}$(__object_instance_counter_get)

endif # __util_object_mk

