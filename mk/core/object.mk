#
#   Date: Dec 20, 2010
# Author: Eldar Abusalimov
#

ifndef __core_object_mk
__core_object_mk := 1

include util/var/assign.mk
include util/var/filter.mk

define __class_class
  $.name    = # The name of the class
  $.fields  = # List of field names
  $.methods = # List of method names

  # 0. this
  # 1. method
  # 2. Class name
  # 3. Unexpanded value of class content
  $,class = \
    ${eval __class_$$_$2 := $0}              \
    $(call $0,name,:=,$2)                    \
    $(foreach 1,$0,                      \
        ${eval $3}                \
        $(call var_list_map,foreach v,$(filter $.%,$(.VARIABLES)), \
        $(call __class_field_handle,$1,$v))

  $,__init_member_filter =

  # Return: string representation
  $,to_string  = $($0.name)

endef

# Params:
#   1. Class object
. = __class_$1_field_$$_
# Params:
#   1. Class object
, = __class_$1_method_$$_

# Params:
#  1. The target variable name
class_check = \
  $(if $(and $(findstring .,$1), \
       $(filter-out simple,$(flavor $1))),$(call __class_check_more,$1,$(subst ., ,$1)))

__class_check_more = $(error $0: $1 assign failed)# TODO

__class_init_core = \

class_register_all = \
  $(call var_list_map,__class_register_filter)

__class_register_filter = \
  $(if $(and $(filter 2,$(words $1)), \
             $(filter class,$(call firstword,$1))), \
       $(call __class_register,$(word 2,$1),$(value $1)))

__class_register = \
  $(call __class_init,$(call new,class),$1,$2)

# 1. Class object
# 2. Class name
# 3. Unexpanded value of class content
__class_init = $(strip \
  ${eval __class_$$_$2 := $1}              \
  ${eval $1.name := $2}                    \
  ${eval $3}                               \
  $(foreach v,$(filter $.%,$(.VARIABLES)), \
      $(call __class_field_handle,$1,$v)) \
)$1

# 1. Struct
# 1. Field variable
__class_field_handle = \
  $(call var:=,$2,$                                                        \
     $(call __field_init,$(call new,field),$1,$(patsubst $.%,%,$2),$($2))) \
  $(call field+=,$1.fields,$2)


# Params:
#   1. Class name
#  ... Constructor arguments
# Return: the newly created object
new = \
  $(or $(foreach 1,$(__class_lookup),$(foreach 0,$(__object_alloc),$ \
           $(__object_init)${eval $$(and $(value $,$($1.name)),)}$0)), \
       $(error $0: class '$1' not found))

# Params:
#   0. Object
#   1. Class
__object_init = \
  ${eval $0 = $(__object_handle_value)}$ \
  $(foreach field,$($1.fields),${eval $0.$(field) := $($.$(field))})

# Params:
#   0. Object
#   1. Class
# Escaped:
#   0. Object in case of method invocation, something else otherwise
#   1. Name of the method being invoked, if any
#  ... Arguments
# Return:
#   The value to assign to a variable named as the object.
__object_handle_value = \
  $$(if $$(filter $0,$$0),$ \
      $${eval __object_tmp := \
          $$(or $$(value $(subst $$,$$$$,$,)$$1),$ \
                $$(error $$0: No such member: '$$1'))$ \
        }$$(__object_tmp),$$(call $0,to_string))

# 1. obj
# 2. clazz
# ret: true/false
instanceof =

# Return: new object identifier.
__object_alloc = \
  ${eval __object_instance_cnt += x}__obj_$(words $(__object_instance_cnt))
__object_instance_cnt :=

endif # __core_object_mk

