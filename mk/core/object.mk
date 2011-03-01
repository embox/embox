#
#   Date: Dec 20, 2010
# Author: Eldar Abusalimov
#

ifndef __core_object_mk
__core_object_mk := 1

include util/var/filter.mk

# Ex:
define class error
  $.message := unknown message
  $.init(1)  = $(if $1,$(call $.set,message,$1))
  $.report() = $(info $($.message))
endef

define class entity
  $.error := $(null)
  $.init() = $(call $.set,error,$(call new,error,Bad entity))
  $.do()   = $(call $($.error),report)
endef


__class_method_$$_error_$$_report

# 1. clazz
# ret: id
new = \
  $(foreach this,$(__object_instance_cnt_get_and_inc),$(call __object_init,$1))

# 1. Class name
__object_init = \
  $(foreach field,$(call __object_class_fields,$1) \
    ,$(call .set,$(field),$(call $(field))))$(call .set,__class,$1)

# 1. Class name
__object_class_fields = \
  $(filter-out %$(\brace_close),$(call __object_class_members,$1))

# 1. Class name
__object_class_members = \
  $(patsubst $1.%,%,$(call var_list_map,__object_class_member_filter,,$1))

# 1. Variable name
# 2. Class name
__object_class_member_filter = $(filter $2.%,$(call singleword,$1))

# 1. obj
# 2. clazz
# ret: true/false
instanceof =

# Params:
#  1. Instance on which to invoke the method
#  2. Method to invoke
# ... Args
invoke =

# Params:
#  1. Method to invoke
# ... Args
# Return: Method invocation result
.invoke = \
  $(call invoke,$(this),$1,$2)

# Params:
#  1. Instance which's field to get
#  2. The field
# Return: The field value
get =

# Params:
#  1. The field
# Return: The field value
.get = \
  $(call get,$(this),$1)

# Params:
#  1. Instance which's field to set
#  2. The field
#  3. The value to set
set =

# Params:
#  1. The field
#  2. The value to set
.set = \
  $(call set,$(this),$1,$2)

# Returns the argument if it denotes a valid object reference, fails otherwise
__object_this_check = \
  $(word $(1:obj%=%),)$(strip $1)

__object_instance_cnt :=
__object_instance_cnt_get = \
  obj$(firstword $(__object_instance_cnt))
__object_instance_cnt_inc = ${eval __object_instance_cnt := \
      $(words x $(__object_instance_cnt)) $(__object_instance_cnt)}
__object_instance_cnt_get_and_inc = \
  $(__object_instance_cnt_get)$(__object_instance_cnt_inc)

endif # __core_object_mk

