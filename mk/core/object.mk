#
#   Date: Dec 20, 2010
# Author: Eldar Abusalimov
#

ifndef __core_object_mk
__core_object_mk := 1

include util/var/assign.mk
include util/var/filter.mk

define struct struct
  $.name   := # the name of the structure
  $.fields := # list of fields
endef

define struct field
  $.struct := # container struct
  $.name   := # field name
  $.value  := # default value
endef

# 1. Object representing the struct
. = __struct_field_$$_$1_$$_

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

#
# Function: field:=
#
# Params:
#  1. Variable name in form instance.field
#  2. The value to set
field$(\colon)$(\equal) = \
  $(if $(findstring simple,$(flavor $1)),$(call var:=,$1,$2), \
      $(call __field_assign_error,$0,$1,$2))

#
# Function: field+=
#
# Params:
#  1. Variable name in form instance.field
#  2. The value to set
field$(\plus)$(\equal) = $(call var+=,$1,$2)

struct_register_all = \
  $(call var_list_map,__struct_register_filter)

__struct_register_filter = \
  $(if $(and $(filter 2,$(words $1)), \
             $(filter class,$(call firstword,$1))), \
       $(call __struct_register,$(word 2,$1),$(value $1)))

__struct_register = \
  $(call __struct_init,$(call new,struct),$1,$2)

# 1. Struct
# 2. Struct name
# 3. Unexpanded value of struct content
__struct_init = $(strip \
  $(call field:=,$1.name,$2)               \
  ${eval $3}                               \
  $(foreach v,$(filter $.%,$(.VARIABLES)), \
      $(call __struct_field_handle,$1,$v)) \
)$1

# 1. Struct
# 1. Field variable
__struct_field_handle = \
  $(call var:=,$2,$                                                        \
     $(call __field_init,$(call new,field),$1,$(patsubst $.%,%,$2),$($2))) \
  $(call field+=,$1.fields,$2)


# 1. Field
# 2. Struct
# 3. Field name
# 4. Default value
__field_init = $(strip \
  $(call field:=,$1.struct,$2) \
  $(call field:=,$1.name,$3)   \
  $(call field:=,$1.value,$4)  \
)$1

# Returns the argument if it denotes a valid object reference, fails otherwise
__object_this_check = \
  $(word $(1:obj%=%),)$(strip $1)

__object_instance_cnt :=
__object_instance_cnt_get = \
  __obj_$(firstword $(__object_instance_cnt))
__object_instance_cnt_inc = ${eval __object_instance_cnt := \
      $(words x $(__object_instance_cnt)) $(__object_instance_cnt)}
__object_instance_cnt_get_and_inc = \
  $(__object_instance_cnt_get)$(__object_instance_cnt_inc)

endif # __core_object_mk

