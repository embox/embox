#
# Copyright 2010-2011, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2010-2011, Lanit-Tercom Inc. All rights reserved.
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

include mk/core/common.mk
include mk/core/string.mk
include mk/core/define.mk

include mk/util/var/assign.mk
#include mk/util/var/list.mk

#
# $(new class,args...)
#
define builtin_func-new
	$$(foreach this,$$(__object_alloc),
		$$(eval \
			$(if $(multiword $(builtin_args_list)),
				$$(call $$(call __class_resolve,$1),$(builtin_nofirstarg)),
				$$(call $$(call __class_resolve,$1))
			)
		)
		$$(this)
	)
endef

# Return: new object identifier
__object_alloc = \
  __obj$(words $(__object_instance_cnt))${eval __object_instance_cnt += x}
# If you change initial value do not forget to modify bootstrap code.
__object_instance_cnt :=# Initially empty.

# Params:
#   1. Class name.
define __class_resolve
	$(if $(findstring undefined,$(flavor class-$1)),
		$(error \
			Class '$1' not found
		),
		class-$1
	)
endef

define builtin_tag-__class__
	$(foreach c,
		$(or \
			$(filter-patsubst class-%,%,$(__def_var)),
			$(error \
					Illegal function name for class: '$(__def_var)')
		),

		${eval \
			$c.methods :=$(\n)
			$c.fields  :=$(\n)
			$c.super   :=
		}

		$c# Return.
	)
endef

define builtin_func-__class__
	$(foreach c,$(call builtin_tag,__class__),
		${eval \
			$c.methods :=$(strip $($c.methods))$(\n)
			$c.fields  :=$(strip $($c.fields))$(\n)
			$c.super   :=$(or $(strip $($c.super)),object)
		}

#		$(if $(eq $($c.super),object),
#			# TODO is it really needed?
#			$$(class-object)$$(\n)
#		)
		$(if $(not $(singleword $($c.super))),
			$(call builtin_error,
					Multiple inheritance for class '$c': \
					$($c.super:%='%',))
		)

		# Invoke super constructor.

		$(builtin_args)
	)
endef
$(call def,builtin_func-__class__)

#
# $(field name,initializer...)
#
define builtin_func-field
	$(assert $(eq __class__,$(builtin_caller)),
		Function '$0' can be used only within a class definition)

	$(if $(or \
			$(not $(singleword $1)),
			$(findstring $(\\),$1),
			$(findstring $(\h),$1),
			$(findstring $$,$1),
			$(findstring  .,$1)),
		$(call builtin_error,
				Illegal field name: '$1')
	)

	${eval \
		$(call builtin_tag,__class__).fields += $1
	}

	# Line feed in output of builtin handler breaks semantics of most other
	# builtins, but as a special exception...
	# We assure that the transformed value will be handled by '__class__'
	# builtin, that will take a special care about it.
	$(\n)
	$$(this).$(trim $1) := \
		$(and \
			$(value 2),
			$(not $(findstring $2x,$(trim $2x))),
			$$(\0)
		)# Preserve leading whitespaces.
		$(subst $(\h),$$(\h),$(subst $(\\),$$(\\),$(builtin_nofirstarg)))
	$(\n)# <- LF again.
endef

##
# Function: new
# Creates a new instance of the specified class.
# Semantics of this function are assumed to be obvious...
#
# Params:
#   1. Class name
#  ... Constructor arguments
# Return:
#      The newly created object.
define new
	# Argument '0' is overridden here with a pointer to object being created.
	$(foreach 0,
		# Assign pointer of class object to '1'.
		$(foreach 1,$(or $(__class_object),
				# Load requested class if it has not been loaded yet.
				$(__class_load),
				# Die here if class loader fails to find a class
				# with the specified name.
				$(error $0: class '$1' not found)
			),
			# Allocate a new object descriptor and assign its value to '0'.
			$(foreach 0,$(__object_alloc),
				# Define a variable named as the descriptor.
				# Also initialize all object fields in a proper order.
				$(__object_init)$0
			)
		),# Pointer to the allocated object.

		# Expand the newly created object handle in the current context:
		#   0. Object
		#   1. Class name (argument of 'new')
		# It is exactly the same as invoking constructor explicitly!
		$(and $($0),)

		# Finally return the pointer stored in '0'.
		$0
	)
endef
$(call def,new)

# Params:
#   0. Object
#   1. Class
#  ... Constructor arguments
define __object_init
	${eval \
		$0 = $(__object_handle_value)
	}
	$(foreach __field,$($1.fields),
		$(foreach __field_name,$(basename $(__field)),
			$(foreach __field_index,$(subst .,,$(suffix $(__field))),
				${eval \
					$(value __object_init_field_mk)
				}
			)
		)
	)
endef
$(call def,__object_init)

# Params:
#   0. Object
#   1. Class
#  ... Constructor arguments
define __object_init_field_mk
  $0.$(__field_name) := \
    $($(call __member_prefix,$1,field$(__field_index))$(__field_name))
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
define __object_handle_value
	$$(if $$(filter $0,$$(value 0)),
		$$(foreach 0,$$(value 1),
			$$(foreach 1,$0,
				$${eval \
					__object_tmp__ :=
						$(foreach m,$(subst $$,$$$$,$($(\comma)))$$0,
							$$(or \
								$$(value $m),
								$$(if $$(findstring undefined,$$(flavor $m)),
									$$(error $$1: \
										No member '$$0' in class '$$($1.name)'
									)
								)
							)
						)
				}
				$$(__object_tmp__)
			)
		),
		$$(call $0,to_string)
	)
endef
$(call def,__object_handle_value)

#     $$(warning invoking $$0.$$1($$(value 2)$ \
#         $$(if $$(value 3),$$(\comma)$$(if $$(findstring $$(\n),$$3),<...>,$$3))))$ \

# Return: new object identifier
__object_alloc = \
  __obj$(words $(__object_instance_cnt))${eval __object_instance_cnt += x}
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
# Return: 'type' as the first word and 'name' as the rest or empty on error
__member_split = \
  $(foreach type, \
      $(call filter-patsubst,$(call __member_prefix,$1,%),%, \
        $(firstword $(subst $$,$$ ,$2))),$(type) \
    $(subst $(call __member_prefix,$1,$(type)),,$2))

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
  $1.fields := $($1.fields:$(call __member_prefix,$1,field$3)=$2.$3)
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
# Class loading.
#

# 1. Class name
# Return: class object or empty if such class is not defined
__class_load = $(strip $(and \
  $(call singleword,$(subst $$,$$ $$,$(subst %,% %,$1))), \
  $(call var_list_map,__class_load_filter,,$1) \
))

# 1. Variable name
# 2. Class name
__class_load_filter = $(and \
  $(call doubleword,$1), \
  $(filter class,$(word 1,$1)), \
  $(filter    $2,$(word 2,$1)), \
  $(if $(call __class_object,$2),$ \
       $(error __class_load: Attempting to reload class '$2'),$ \
  ${eval $$(call __class,$2) := $(call new,class,$2,$(value $1))}$ \
     $(call __class_object,$2)) \
)

# 1. Class name
__class = \
  __class_$$$1

# 1. Class name
# Return: class object or empty if such class is not loaded
__class_object = \
  $(value $(__class))

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
    $(or $(call $1,__init_member_split,$(__member_split)), \
         $(error $($1): Invalid member: '$2'))

  # 1. This
  # 2. Member ('type' 'name') or empty if type is not recognized
  # Return: Member name if it is ok
  $,__init_member_split = \
     $(call $1,__init_member,$(call firstword,$2),$(call nofirstword,$2))

  # 1. This
  # 2. Member type ('method' or 'field000')
  # 3. Member name
  # Return: Member name if it is ok
  $,__init_member = $(and \
     $(or $2, \
          $(warning $($1): Unknown member type)), \
     $(or $(filter method field%,$2), \
          $(warning $($1): Unrecognized member type: '$2')), \
     $(or $3, \
          $(warning $($1): Empty member name)), \
     $(or $(call singleword,$(subst $$,$$ $$,$3)), \
          $(warning $($1): Invalid member name: '$3')), \
     $(or $(filter-out $($1.members),$3), \
          $(warning $($1): Redefinition of member '$3')), \
     $(or $(filter method,$2), \
          $(call $1,__init_field,$3,$(call __field_index,$2))), \
     ${eval $$1.members += $$3}$3 \
   )

  # 1. This
  # 2. Field name
  # 3. Field index
  # Return: Member name if it is ok
  $,__init_field = \
    $(and $3,$(filter $(call __member_prefix,$1,field$3),$($1.fields)), \
          ${eval $(value __field_init_mk)}$2)

  # Return: string representation
  $,to_string  = Class '$($1.name)'

endef

# 1. This
# 2. Class name
# 3. Unexpanded value of class content
__class_init = \
  ${eval $$1.name := $$2} \
  ${eval $$1.members :=} \
  ${eval $$1.fields :=} \
  $(call var_list_filter_out_invoke,__init_member_filter,$1, \
      $(.VARIABLES)${eval $$$(\comma)$2 :=$(\n)$3})
#
# Bootstrap...
#

ifeq (0,1)
# The first created object will be the instance of class 'class'.
# It is not allocated as usual, but we make it so that its descriptor equals
# to an object which will be allocated by the first call to 'new'.
$(call __class,class) := __obj0# In respect to object allocation logic.

# Some parts of class instance for 'class' must be prepared by hand in order to
# the first 'new' could perform its job properly. These include constructor and
# fields (both default values and setters).
$(foreach 1,$(call __class_object,class), \
  ${eval $$,class = $$(__class_init)} \
  $(foreach 2,fields members name,${eval $$1.$$2 := }) \
)

# Now we can use regular 'new'.
$(if $(filter-out \
         $(call new,class,class,$(value __class_class)), \
         $(call __class_object,class)), \
     $(error Something went wrong during bootstrap of object subsystem))
endif

$(def_all)

endif # __core_object_mk
