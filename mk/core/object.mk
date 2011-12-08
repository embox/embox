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

include mk/core/common.mk
include mk/core/string.mk
include mk/core/define.mk

include mk/util/var/assign.mk

#
# $(new class,args...)
#
define builtin_func-new
	$$(foreach __class__,$$(call __class_resolve,$1),
		$(def-ifdef OBJ_DEBUG,
			$$(foreach __obj_debug_args_nr,
				$(words $(nofirstword $(builtin_args_list))),
				$$(call __new,$(builtin_nofirstarg))
			),
			$(if $(multiword $(builtin_args_list)),
				$$(call __new,$(builtin_nofirstarg)),
				$$(call __new)
			)
		)
	)
endef

# Return:
#   Human-readable list of args from 1 up to '__obj_debug_args_nr',
#   or 'no args' if '__obj_debug_args_nr' is 0.
# Note:
#   Do not 'call'.
define __obj_debug_args
	$(or \
		$(foreach a,
			$(wordlist 1,$(__obj_debug_args_nr),1 2 3 4 5 6 7 8 9),
			$(\t)$$$a='$($a)'
		),
#		<no args>
	)
endef

# Params:
#   1.. Args...
# Context:
#   '__class__'
# Return:
#   New object identifier.
define __new
	$(foreach this,__obj$(words $(__object_instance_cnt)),
		$(def-ifdef OBJ_DEBUG,
			$(info \
					$(this): new $(__class__): $(__obj_debug_args))
		)
		${eval \
			__object_instance_cnt += x
			$(\n)
			$(this) := $(__class__)
			$(\n)
			$(class-$(__class__))
		}
		$(this)
	)
endef
__object_instance_cnt :=# Initially empty.

#
# Runtime member access: 'invoke', 'get' and 'set'.
#

# Params:
#   1. Member name to parse in form 'member', 'ref->member' or 'obj.member'.
#      It is assumed that there is no any commas outside parens.
#   2. Continuation with the following args:
#       1. Empty in case of referencing a member of 'this',
#          the target object otherwise.
#       2. The member being referenced.
#       3. Optional argument.
#   3. (optional) Argument to pass to the continuaion.
# Return:
#   Result of call to continuation in case of a valid reference,
#   otherwise it aborts using builtin_error.
define __object_member_parse
	$(or \
		$(__object_member_try_parse),
		$(call builtin_error,
			Invalid first argument to '$(builtin_name)' function: '$1'
		)
	)
endef

# Params:
#   See '__object_member_parse'.
# Return:
#   Result of call to continuation in case of a valid reference,
#   empty otherwise.
define __object_member_try_parse
	$(expand $$(call \
		$(lambda $(or \
			$(and \
				$(eq .,$6),$(nolastword $4),$(trim $5),
				# Invocation on some object.
				# Escaped reference is in $4. Escaped member name is in $5.
				$(call $1,
					# Optimize out explicit dereference of 'this'.
					$(if $(not $(eq this ->,$(strip $4))),
						$(call $(if $(eq ->,$(lastword $4)),$(lambda $$($1)),id),
							$(call $3,$(nolastword $4))
						)
					),
					$(call $3,$5),$2)
			),
			$(and \
				$(eq .,$5),$(trim $4),
				# Target object is implicitly 'this'.
				# Escaped member is in $4.
				$(call $1,,$(call $3,$4),$2)
			)
		)),

		# 1 and 2: The continuation with its argument.
		$$2,$$(value 3),

		# 3: Unescape function which restores '.' and '->' back.
		$(lambda \
			$(trim $(subst $(\s).$(\comma),.,$(subst $(\s)->$(\comma),->,$1)))
		),

		# 4 .. 5: Escaped member name with '.' and '->' repalaced by commas.
		$(subst .,$(\s).$(\comma),$(subst ->,$(\s)->$(\comma),
			$(subst $(\comma),$$(\comma),$(subst $$,$$$$,$1))
		)),

		# 5 or 6: End of args marker.
		.,
	))
endef

# Params:
#   1. The target object or empty in case of referencing a member of 'this'.
#   2. The code to wrap.
# Return:
#   The code (wrapped if needed).
define __object_member_access_wrap
	$$(foreach __this,$(if $1,$$(call __object_check,$1),$$(this)),
		$2
	)
endef

$(def_all)

# Params:
#   1. Object reference.
# Return:
#   The trimmed argument if it is a single word, fails with an error otherwise.
define __object_check
	$(or \
		$(singleword $1),
		$(error \
				Invalid object reference: '$1')
	)
endef

#
# $(invoke method,args...)
# $(invoke obj.method,args...)
# $(invoke ref->method,args...)
#
define builtin_func-invoke
	$(call __object_member_parse,$1,
		# 1. Empty for 'this', target object otherwise.
		# 2. Referenced method.
		# 3. Args...
		$(lambda \
			$(call __object_member_access_wrap,$1,
				$(def-ifdef OBJ_DEBUG,
					$$(foreach __args_nr,
						$(words $(builtin_args_list)),
						$$(call __method_invoke,$3,$2)
					),
					$$(call $$($$(__this)).$2,$3)
				)
			)
		),
		$(builtin_nofirstarg)
	)
endef

# Params:
#   1..N: Args...
#   N+1:  Method
# Context:
#   '__class__'
ifdef OBJ_DEBUG
define __method_invoke
	$(foreach __obj_debug_args_nr,$(wordlist 2,$(__args_nr),x 1 2 3 4 5 6 7 8 9),
		$(info \
				$(__this): invoke $($(__this)).$($(__args_nr)): \
				$(__obj_debug_args))
	)
	$(foreach 0,$($(__this)).$($(__args_nr)),
		$(expand $(value $0))
	)
endef
endif

$(def_all)

# Params:
#   1. Field name.
# Context:
#   '__this'
define __field_check
	$(if $(findstring simple,$(flavor $(__this).$1)),
		$1,
		$(error \
				Invalid field name: '$1', referenced on object '$(__this)')
	)
endef

# Expanded from field 'setxxx' builtin context. It will generate a call
# to '_field_setxxx', where xxx is taken from builtin name.
define __builtin_func_set
	$(call builtin_check_min_arity,2)
	$(call __object_member_parse,$1,
		# 1. Empty for 'this', target object otherwise.
		# 2. Referenced field.
		# 3. Value.
		$(lambda \
			$(call __object_member_access_wrap,$1,
				$$(call __field_$(builtin_name),$$($$(__this)),$2,$3)
			)
		),
		$(builtin_nofirstarg)
	)
endef

#
# $(set field,value)
# $(set obj.field,value)
# $(set ref->field,value)
#
define builtin_func-set
	$(__builtin_func_set)
endef

# Params:
#   1. Class.
#   2. Field name.
#   3. Field value.
# Context:
#   '__this'
define __field_set
	${eval \
		override $(__this).$(call __field_check,$2) := \
			$(if $(value $1.set.$2),
				$$(call $1.set.$2,$$3),
				$$3
			)
	}
endef

#
# $(set+ field,value)
# $(set+ obj.field,value)
# $(set+ ref->field,value)
#
define builtin_func-set+
	$(__builtin_func_set)
endef

# Params:
#   1. Class.
#   2. Field name.
#   3. Field value.
# Context:
#   '__this'
define __field_set+
	${eval \
		$(if $($(__this).$(call __field_check,$2)),
			$(if $(value $1.set.$2),
				override $(__this).$2 := \
					$$(call $1.set.$2,$$($(__this).$2) $$3),
					$$3
				,# else
				override $(__this).$2 += \
					$$3
			)
			,# else
			override $(__this).$2 := \
				$(if $(value $1.set.$2),
					$$(call $1.set.$2,$$3),
					$$3
				)
		)
	}
endef

#
# $(get field)
# $(get obj.field)
# $(get ref->field)
#
define builtin_func-get
	$(call builtin_check_max_arity,1)
	$(call __object_member_parse,$1,
		# 1. Empty for 'this', target object otherwise.
		# 2. Referenced field.
		$(lambda \
			$(call __object_member_access_wrap,$1,
				$$(call __field_get,$$($$(__this)),$2,$3)
			)
		)
	)
endef
$(call def,builtin_func-get)

# Params:
#   1. Class.
#   2. Field name.
# Context:
#   '__this'
define __field_get
	$(if $(value $1.get.$(call __field_check,$2)),
		$$(call $1.get.$2,$($(__this).$2)),
		$($(__this).$2)
	)
endef

$(def_all)

# Params:
#   1. Class name.
define __class_resolve
	$(if $(findstring undefined,$(flavor class-$1)),
		$(call $(if $(value __def_var),builtin_)error,
				Class '$1' not found),
		$1
	)
endef

#
# Class definition.
#

define __class_variable_value_provider
	$(assert $(filter class-%,$1))

	$(if $(not $(call __class_name_check,$(1:class-%=%))),
		$(error \
				Illegal class name: '$(1:class-%=%)')
	)

	$$(__class__ $(value $1))
endef
$(call def,__class_variable_value_provider)
$(call def_register_value_provider,class-%,__class_variable_value_provider)

define builtin_tag-__class__
	$(__def_var:class-%=%)
endef

#
# $(__class__ fields/methods/supers...)
#
define builtin_func-__class__
	$(call builtin_check_max_arity,1)

	$(foreach c,$(call builtin_tag,__class__),
		$(and $(foreach a,fields methods super,
			${eval \
				$c.$a := $(strip $(value $c.$a))
			}
		),)
	)

	$1
endef

# Params:
#   1. Class or member name to check.
# Returns:
#   The argument if it is a valid name, empty otherwise.
define __class_name_check
	$(if $(not \
			$(or \
				$(findstring $(\\),$1),
				$(findstring $(\h),$1),
				$(findstring $$,$1),
				$(findstring  .,$1)
			)),
		$(singleword $1)
	)
endef

# Params:
#   1. Class or member name to check.
# Returns:
#   The argument if it is a valid name, fails with an error otherwise.
define __class_name_check_or_die
	$(or $(__class_name_check),
		$(call builtin_error,
				Illegal name: '$1')
	)
endef

# Param:
#   1. Class attribute to append the identifier to.
#   2. Identifier to check.
define __class_def_attribute
	# XXX

	$(__class_def_attribute_no_check)
endef

# Param:
#   The same as to '__class_def_attributes'.
define __class_def_attribute_no_check
	$(assert $(eq __class__,$(builtin_caller)),
		Function '$(builtin_name)' can be used only within a class definition)

	${eval \
		$(call builtin_tag,__class__).$1 += $2
	}
endef

# Defines a new member (method or field initializer) in a specified class.
# Params:
#   1. Class.
#   2. Member name.
#   3. Function body.
define __member_def
	${eval \
		$1.$2 = \
			$(if $(not $(findstring $3x,$(trim $3x))),
					$$(\0))# Preserve leading whitespaces.
			$(subst $(\h),$$(\h),$(subst $(\\),$$(\\),$3))
	}
endef

#
# $(field name,initializer...)
#
define builtin_func-field
	$(foreach 1,$(call __class_name_check_or_die,$1),
		# Arg 1 is got trimmed.

		$(call __class_def_attribute,fields,$1)

		$(call __member_def,$(call builtin_tag,__class__),$1,
				$(builtin_nofirstarg))

		# Line feed in output of builtin handler breaks semantics of most other
		# builtins, but as a special exception...
		# We assure that the transformed value will be handled by '__class__'
		# builtin, that will take a special care about it.
		$(\n)# <- LF.
		$$(this).$1 := $$(value $(call builtin_tag,__class__).$1)
		$(\n)# <- LF again.
	)
endef

#
# $(setter name,body...)
#
define builtin_func-setter
	$(call builtin_check_min_arity,2)

	$(if $(not $(call __class_name_check,$1)),
		$(call builtin_error,
				Illegal name: '$1')
	)

	$(call __class_def_attribute_no_check,methods,set.$(trim $1))

	$(call __member_def,$(call builtin_tag,__class__),set.$(trim $1),
			$$(foreach this,$$(__this),$(builtin_nofirstarg)))
endef

#
# $(method name,body...)
#
define builtin_func-method
	$(call __class_def_attribute,methods,$1)

	$(call __member_def,$(call builtin_tag,__class__),$(trim $1),
			$$(foreach this,$$(__this),$(builtin_nofirstarg)))
endef

#
# $(super ancestor,args...)
#
define builtin_func-super
	$(call __class_def_attribute,super,$1)

	$(foreach c,class-$(call __class_resolve,$1),
		$(if $(multiword $(builtin_args_list)),
			$$(call $c,$(builtin_nofirstarg)),
			$$(call $c)
		)
	)

	$(call __class_inherit,$(call builtin_tag,__class__),$1)

	$(foreach m,$($1.methods),
		$(call __member_def,$(call builtin_tag,__class__),$m,$(value $1.$m))
	)
endef

# Params:
#   1. Child class.
#   2. Ancestor.
define __class_inherit
	$(and $(foreach a,fields methods super,
		${eval \
			$1.$a += $($2.$a)
		}
	),)

	$(if $(filter $1,$($1.super)),
		$(call builtin_error,
				Can't inherit class '$1' from '$2' because of a loop)
	)
endef

$(def_all)

endif # __core_object_mk
