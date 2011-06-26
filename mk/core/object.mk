#
# Copyright 2008-2011, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2008-2011, Lanit-Tercom Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

#
# Object-oriented make.
#
#   Date: Dec 20, 2010
# Author: Eldar Abusalimov
#

ifndef __core_object_mk
__core_object_mk := 1

##
# Usage example:
#
##
#  include core/object.mk
#
#  define class foo
#
#    ##
#    # This is a field. Every instance of class has its own field value.
#    #
#    # There also can be a default value which is assigned to the field at the
#    # time of object creation. The default value is expanded each time a new
#    # object is constructed, and the expansion result is then assigned to the
#    # object's field. Expansion context is much like the constructor call
#    #
#    # Fields are denoted with $. prefix.
#    #
#    $.bar = Default value
#
#    # Fields can be defined verbatim
#    define $.baz
#      Multi-line value, \
#       where $0 expands to 'foo', and $1 - to this - a pointer to the object being constructed
#
#    # This is the constructor.
#    # Fields are denoted with $. prefix.
#    $,foo = Default value, where $0 expands to 'foo', and $1 - to this
#
#  endef
#

include core/common.mk
include core/string.mk

include util/var/assign.mk
include util/var/list.mk

##
# Function: new
# Creates a new instance of the specified class.
# Semantics of this function are assumed to be obvious...
#
# Params:
#   1. Class name (implicitly used in __class)
#  ... Constructor arguments
# Return:
#      The newly created object.
new = \
  $(foreach 0,$(or \
        $(foreach 1,$(value $(__class)),$(foreach 0,$(__object_alloc),$ \
             $(__object_init)$0)), \
        $(error $0: class '$1' not found)) \
     ,$(and $($0),)$0)

# Params:
#   0. Object
#   1. Class
#  ... Constructor arguments
__object_init = $(strip \
  ${eval $0 = $(__object_handle_value)} \
  $(foreach __field,$($1.fields), \
    $(foreach __field_name,$(basename $(field)), \
     $(foreach __field_index,$(suffix $(field)), \
       ${eval $(value __object_init_field_mk)} \
    ))) \
)

# Params:
#   0. Object
#   1. Class
#  ... Constructor arguments
define __object_init_field_mk
  $0.$(__field_name) := $($(call __member_prefix,$1,field$(__field_index)))
endef

# Params:
#   0. Object
#   1. Class (implicitly used inside $,)
# Escaped:
#   0. Object in case of method invocation, something else otherwise
#   1. Name of the method being invoked, if any
#  ... Arguments
# Return:
#   The value to assign to a variable named as the object.
__object_handle_value = \
  $$(if $$(filter $0,$$(value 0)),$ \
      $$(warning invoking $$0.$$1($$(value 2)$ \
          $$(if $$(value 3),$$(\comma)$$(if $$(findstring $$(\n),$$3),<...>,$$3))))$ \
      $$(foreach 0,$$(value 1),$$(foreach 1,$0,$ \
          $${eval __object_tmp__ := $$(or \
                $$(value $(subst $$,$$$$,$(,))$$0),$ \
                $$(error $$1: No such member: '$$0'))}$ \
      $$(__object_tmp__))),$$(call $0,to_string))

# Return: new object identifier
__object_alloc = \
  __obj_$(words $(__object_instance_cnt))${eval __object_instance_cnt += x}
# If you change initial value do not forget to modify bootstrap code.
__object_instance_cnt :=# Initially empty.

#
# Generic member definitions and name checking.
#

# 1. Class
. = $(foreach 0,$(call __member_prefix,$1,$(__field_alloc)),${eval \
        $$1.fields += $$0}$0)

# 1. Class
, = $(call __member_prefix,$1,method)

# 1. Class
# 2. Member type ('field000' or 'method')
__member_prefix = __member$1_$2_$$

# 1. Class
# 2. Member variable name
# Return: Member type or empty on error
__member_type = \
  $(word 1,$(__member_split))

# 1. Class
# 2. Member variable name
# Return: Member name or empty on error
__member_name = \
  $(word 2,$(__member_split))

# 1. Class
# 2. Member variable name
# Return: value of 2 words ('type' 'name') or empty on error
__member_split = \
  $(and $(call singleword,$2), \
        $(call __member_split_check,$1,$(subst $$,$$ $$,$2)))

# 1. Class
# 2. Member variable name splitted into 2 words
# Return: value of 2 words ('type' 'name') or empty on error
__member_split_check = \
  $(and $(filter 2,$(words $2)), \
        $(call __member_check,$(call filter-patsubst,$ \
                   $(call __member_prefix,$1,%),%,$(word 1,$2)),$ \
                     $(call filter-patsubst,$$%,%,$(word 2,$2))))

# 1. Supposed member type
# 2. Supposed member name
# Return: value of 2 words ('type' 'name') or empty on error
__member_check = \
  $(and $1,$2,$1 $2)

#
# Field specific.
#

# 1. Field member type ('field000')
# Return: extracted field index ('000') or empty on error
__field_index = \
  $(call filter-patsubst,field%,%,$1)

__field_alloc = \
  field$(words $(__field_index_cnt))${eval __field_index_cnt += x}
__field_index_cnt :=

# 1. Class
# 2. Field name
# 3. Field index
define __field_init_mk
  # Replace period function expansion ('$.') in value of 'fields'
  # with field name and index separated by period (name.index).
  $1.fields := $($1.fields:$(call __member_prefix,$1,$3)=$2.$3)
  # Define the corresponding getter/setter.
  $,$2 = $(__field_handle)
endef

# Params:
#   0. Field name (the handle itself)
#   1. This
#   2. Set operator, if any
#   3. Value being set, if any
# Return:
#   The field value if set operator is omited, empty otherwise.
__field_handle = \
  $(if $(value 2),$ \
      $(and $(or $(call __field_set_$(subst :,,$(strip $2)),$1.$0,$(value 3)),\
                 $(error $1.$0: Invalid operator: '$2')),),$ \
      $($1.$0))

# __field_set_xxx
$(foreach op,+= -= *= ?=, \
    $(call var_assign_recursive,__field_set_$(op),$(value var$(op))x))
# __field_set_= is a special case
$(call var_assign_recursive,__field_set_=,$(value var:=)x)

#
# Class registering.
#

# Must be called after all classes have been defined.
class_register_all = \
  $(call var_list_map,__class_register_filter)

# 1. Variable name
__class_register_filter = \
  $(if $(and $(filter 2,$(words $1)), \
             $(filter class,$(call firstword,$1))), \
       $(call __class_register,$(word 2,$1),$(value $1)))

# 1. Class name
# 2. Value of class variable
__class_register = \
  $(if $(value $(__class)), \
      $(error class_register_all: Redefinition of class '$1'), \
      ${eval $(__class) := $(call new,class,$1,$2)})

# 1. Class name
__class = \
  __class_$$$1

#
# Class 'class' handles all classes except itself.
#
define __class_class

  # The name of the class
  $.name    =

  $.members =
  $.fields  =

  # 1. This
  # 2. Class name
  # 3. Unexpanded value of class content
  $,class = \
    $(__class_init)

  # 1. This
  # 2. Class variable
  # Return: Member name if it is ok
  $,__init_member_filter = \
    $(or $(call $1,__init_member_check,$(call __member_split,$1,$2)), \
         $(error $($1): Invalid member: '$2'))

  # 1. This
  # 2. Composite member name: 'type' 'name'
  # Return: Member name if it is ok
  $,__init_member_check = \
    $(and $2,$(call $1,__init_member,$(word 1,$2),$(word 2,$2)))

  # 1. This
  # 2. Member type ('method' or 'field000')
  # 3. Member name
  # Return: Member name if it is ok
  $,__init_member = \
    $(and $(or $(filter method field%,$2), \
               $(info $($1): Unrecognized member type: $2)), \
          $(or $(filter-out $($1.members),$3), \
               $(info $($1): Conflicting name for member: $3)), \
          $(or $(filter method,$2),$ \
               $(call $1,__init_field,$3,$(call __field_index,$2))), \
          ${eval $$1.members += $$3}$3)

  # 1. This
  # 2. Field name
  # 3. Field index
  # Return: Member name if it is ok
  $,__init_field = \
    $(and $3,$(filter $(call __member_prefix,$1,field$3),$($1.fields)), \
          ${eval $(value __field_init_mk)}$2)

  # Return: string representation
  $,to_string  = $($1.name)

endef

# 1. This
# 2. Class name
# 3. Unexpanded value of class content
__class_init = \
  $(call var_list_filter_out_invoke,__init_member_filter,$1, \
      $(.VARIABLES)${eval $3}) \
  $(call $1,name,=,$2)

#
# Bootstrap...
#

# The first object will represent the instance of class 'class'.
# It is not allocated as usual, and it equals to an object which will be
# allocated when we'll call 'new' for the first time namely 'new,class,class'.
__class_object := __obj_0
$(call __class,class) := $(__class_object)

# Some parts of class instance for 'class' must be prepared by hand in order to
# the first 'new' could perform its job properly. These include constructor and
# fields (both default values and setters).
$(__class_object).fields := fields members name
$(foreach 1,$(__class_object), \
  ${eval $$,class = $$(__class_init)} \
  $(foreach 2,$($1.fields), \
    ${eval $$1.$$2 := } \
  ) \
)
#    ${eval $$(,)$2 = $(__field_handle_value)} \

# Now we can use regular 'new'.
__class_object := $(call new,class,class,$(value __class_class))
$(if $(filter-out __obj_0,$(__class_object)), \
  $(error Something went wrong during bootstrap of object subsystem))

endif # __core_object_mk
