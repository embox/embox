#
# Copyright 2010-2012, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2010-2012, Lanit-Tercom Inc. All rights reserved.
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
# General overview.
#
#   This file provides an object-oriented superstructure for Make.
#
##
# Terminology.
#
#   Class
#     - a prototype of each instance (object), which defines members (fields
#       and methods) available for instances.
#
#   Object
#     - an instance of a class. Each object has its own set of values of
#       fields associated with the class of the object.
#
#   Object reference
#     - a value used to refer an object.
#
#   Method
#     - a function associated with a class, defines runtime behavior of an
#       instance of the class. Method is evaluated in a special context with
#      'this' pointing the current object, thus it has as access to other
#       members of the object.
#
#   Field
#     - a value associated with each object of a certain class, provides a
#       runtime state of the instance.
#
#   Property
#     - a special kind of member representing a runtime state of the instance
#       through getter and setter methods.
#
#   Reference field/property
#     - a member, that holds references to other objects.
#
##
# Class definition.
#
#   Each variable with name starting with 'class-' is considered as a new class
#   definition. The name of the class is everything after the dash.
#
#   	define class-clazz
#   		...
#   	endef
#
#   In the example above, a variable 'class-clazz' defines a new class
#   named 'clazz'. The name of the class is primarily used for object
#   instantiation and for class introspection.
#
#   So what can class actually define?
#
#
# Field definition.
#
#   A field is defined using special 'field' builtin function.
#
#   	$(field name[...][:{*|type}][,initializer...])
#
#   The first argument specifies the name of the field and whether the field
#   is a reference field. In the latter case one can also specify a type of
#   objects that can be held in this field.
#
#   If an initializer is specified in the second argument, it will be called
#   every time when a new object is created, and its return value will be
#   assigned to the field. Otherwise, the field gets empty initial value
#   by default.
#
#   	# A simple field for storing an arbitrary data.
#   	$(field a_field)
#
#   	# A field with an initializer. Note the usage of 'this'.
#   	$(field a_field_with_initializer,
#   		$(info initializing a field of '$(this)')
#   		initial value)
#
#   	# A field that holds references to any objects.
#   	$(field a_reference_field : *)
#
#   	# A reference fields that enforces the type of referenced objects to
#   	# be 'some_class'.
#   	$(field a_reference_field : some_class)
#
#
# Method definition.
#
#   As fields methods are defined using special builtin function, which has the
#   following syntax.
#
#   	$(method name[,implementation...])
#
#   The first argument is the name of the method.
#
#   If the second argument is not specified, then the method is considered
#   abstract. Abstract method are assumed to be implemented in some subclass.
#   To prevent invocation of an abstract method a special stub ($(error ...))
#   is assigned as its body.
#
#   	# A simple method which always returns 'result' string.
#   	$(method a_method,result)
#
#   	# A method with no implementation (abstract method).
#   	$(method an_abstract_method)
#
#   	# As a usual function method body have an access to all passed
#   	# arguments ($1 ..). A reference to the current object is available
#   	# in 'this' variable.
#   	$(method say_hello,
#   		$(info Hello from '$(this)', the first argument is '$1'))
#
#
# Property and its accessors definition.
#
#   The definition of a property is very similar to the fields except that
#   there is no initializer function.
#
#   	$(property name[...][:{*|type}])
#
#   As for fields the property can be defined to hold object references.
#
#
# Class inheritance.
#
#   Class can extend other classes, inheriting their members. Multiple
#   inheritance is supported, tracking an inheritance order and avoiding
#   MI issues (such as diamond problem) is the responsibility of user.
#
#   	$(super class_name[,arguments...])
#
#   The first argument is the name of a class to inherit. The specified class
#   must have already been defined. Arguments (if any) are passed to a
#   constructor of the super class.
#
#
# Constructor statements.
#
#   There is no dedicated structure for a constructor. Instead, the whole
#   class body is considered as a constructor. That is, you may include an
#   arbitrary code outside the builtins mentioned above.
#
#   Instantiation also involves field initialization and calling super
#   constructors, in the order they have been defined the class body.
#
#   	define class-zuper
#   		$(info Super constructor with argument '$1')
#
#   		$(field delegate : zuper)
#
#   	endef
#
#   	define class-clazz
#   		$(info This is executed first)
#
#   		$(super zuper,
#   			Hello from clazz)
#
#   		$(field foo,
#   			$(info Initializing field 'foo')
#   			initial value)
#
#   		$(method bar,
#   			$(info Calling method 'bar')
#   			hello world)
#
#   		$(info End of constructor)
#   	endef
#
#   In this example the following test will be printed during constructing
#   a new object:
#
#   	This is executed first
#   	Super constructor with argument 'Hello from clazz'
#   	Initializing field 'foo'
#   	End of constructor
#
##
# Runtime concepts.
#
#   After a class has been defined, one can create objects of such class,
#   invoke its methods and get/set field values.
#
#
# Object instantiation.
#
#   A new object is created using 'new' builtin function.
#
#   	$(new class_name[,arguments...])
#
#   The first argument specifies the name of a class being instantiated.
#
#   Note that this should be the name of the class itself, not the name of a
#   variable used to define a class. The name of the class defined in
#   variable 'class-clazz' is 'clazz', and a proper call to 'new' is the
#   following:
#
#   	$(new clazz)
#
#   Arguments specified after a class name are passed to the constructor.
#
#  'new' allocates a unique object identifier which acts as the object
#   reference and is accessible within the constructor through 'this' variable.
#   This identifier is then returned as the result value of 'new'.
#
#
# Accessing object members.
#
#   Member access includes method invocation and getting/setting field values.
#
#   	# Calls the method with given arguments and returns the result.
#   	$(invoke method[,arguments...])
#
#   	$(get-field  field)       # Retrieves the current field value.
#
#   	$(set-field  field,value) # Assigns a new value to the field.
#   	$(set-field+ field,value) # Appends a given value to the field.
#   	$(set-field- field,value) # Removes any occurrence of a value.
#
#   	$(get  property)          # Invokes getter of the specified property.
#
#   	$(set  property,value)    # Sets a new value by calling setter.
#   	$(set+ property,value)    # Append (multi-value properties only).
#   	$(set- property,value)    # Remove (multi-value properties only).
#
#   Related builtin functions allows one to specify a target member in three
#   different forms:
#
#   member
#     - Simple form for referring members of 'this' object.
#
#   <object>.member
#     - Accesses a member of the specified object.
#
#   	$(invoke $(new clazz).some_method)
#
#   	# A more real world example.
#   	$(foreach object,$(new clazz),
#   		$(invoke $(object).some_method))
#
#   <variable>->member
#     - The same as the previous one, but the object reference is computed
#       by expanding the specified variable. In fact, this form is a shorthand
#       for '$(<variable>).member'. Thus, the last of the examples above
#       could rewritten as follows:
#
#   	$(foreach object,$(new clazz),
#   		$(invoke object->some_method))
#
#   It is important to note that the latter two forms must include '.' (period)
#   and '->' (dash with closing angle bracket) literally. Think of them as a
#   kind of separator, such as ',' (comma) for arguments.
#
#
# A few words about object references.
#
#   A reference to an object is a string that is used to identify the object.
#   For example, 'new' returns a reference, 'this' contains a reference (in
#   certain contexts, of course: within constructors and methods).
#
#   There is, however, a special feature of references: prepending an
#   arbitrary prefix does not corrupt the reference. Moreover, one can extract
#   a prefix (if any) efficiently using $(basename <ref>) function.
#   In turn a normalized reference is available through $(suffix <ref>).
#
#   So, returning to an example of method invocation, one could modify it as
#   follows without changing the meaning:
#
#   	$(foreach object,foo/bar.baz!$(new clazz),
#   		$(invoke $(object).some_method))
#
##
# Internal representation.
#
#  TODO outdated. -- Eldar
#
#
#   During class definition or a new object instantiation a number of ancillary
#   variables are created.
#
#   Considering the code below as an example, we'll examine what variables
#   are created and when.
#
#   	define class-zuper
#   		$(info Super constructor with argument '$1')
#
#   		$(field delegate : zuper)
#
#   	endef
#
#   After defining class 'zuper' variable 'class-zuper' becomes a part
#   of its constructor. Also an initializer function for 'delegate' field, a
#   type checking setter method for it and three special variables are defined.
#
#   	# class-zuper = \
#   	# 	$(info Super constructor with argument '$1') \
#   	# 	$(eval $(this).delegate := $(value zuper.delegate))
#
#   	# zuper.super :=
#   	# zuper.fields := delegate.zuper
#   	# zuper.methods := set.delegate
#
#   	# zuper.field.delegate =
#   	# zuper.method.set.delegate = \
#   	# 	$(foreach 2,zuper,$(__field_setter_type_check))
#
#   Lets extend our class as follows:
#
#   	define class-clazz
#   		$(info This is executed first)
#
#   		$(super zuper,
#   			Hello from clazz)
#
#   		$(field foo,
#   			$(info Initializing field 'foo')
#   			initial value)
#
#   		$(method bar,
#   			$(info Calling method 'bar')
#   			hello world)
#
#   		$(info End of constructor)
#   	endef
#
#   Notice additional two variables for field 'foo' and method 'bar':
#
#   	# class-clazz = \
#   	# 	$(info This is executed first) \
#   	# 	$(call class-zuper,Hello from clazz) \
#   	# 	$(eval $(this).foo := $(value clazz.foo)) \
#   	# 	$(info End of constructor)
#
#   	# clazz.super := zuper
#   	# clazz.fields := delegate.zuper foo
#   	# clazz.methods := set.delegate bar
#
#   	# clazz.field.foo = \
#   	# 	$(info Initializing field 'foo')initial value
#   	# clazz.method.bar = \
#   	# 	$(foreach this,$(__this),$(info Calling method 'bar')hello world)
#   	# clazz.method.set.delegate = \
#   	# 	$(foreach 2,zuper,$(__field_setter_type_check))
#
#   Finally, we will instantiate 'clazz':
#
#   	inztance := $(call new,clazz)
#
#   And inspect newly defined variables:
#
#   	# inztance := .obj1
#
#   	# .obj1 := clazz
#   	# .obj1.delegate :=
#   	# .obj1.foo := initial value
#
#

include mk/core/alloc.mk
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
$(call def,builtin_func-new)

# Creates a new instance of the specified class.
# Only a single argument is passed to the constructor.
#   1. Class name.
#   2. An (optional) argument to pass to the constructor.
# Return:
#   Newly created object instance.
new = \
	$(new $1,$(value 2))

# Params:
#   1.. Args...
# Context:
#   '__class__'
# Return:
#   New object identifier.
define __new
	# It is mandatory for object references to start with a period.
	$(foreach this,.obj$(call alloc,obj),
		$(__obj_trace_push $(this),new    $(__class__): $(__obj_debug_args))
		${eval \
			$(this) := $(__class__)$(\n)
			# Call the constructor.
			$$(and $(value class-$(__class__)),)
		}
		$(__obj_trace_pop $(this),new    $(__class__))
		$(this)
	)
endef

ifdef OBJ_DEBUG

__obj_trace_stack :=

# Param:
#   1. Obj.
#   2. Msg.
__obj_trace = \
	$(warning $(__obj_trace_stack:%=>$(\t))$1:	$2)

# Param:
#   1. Obj.
#   2. Msg.
define __obj_trace_push
	$(__obj_trace)
	${eval __obj_trace_stack += x}
endef

# Param:
#   1. Obj.
#   2. Msg.
define __obj_trace_pop
	${eval __obj_trace_stack := $(nofirstword $(__obj_trace_stack))}
	$(__obj_trace)
endef

builtin_func-__obj_trace      = $(builtin_to_function_call)
builtin_func-__obj_trace_push = $(builtin_to_function_call)
builtin_func-__obj_trace_pop  = $(builtin_to_function_call)

$(def_all)

else
builtin_func-__obj_trace      :=
builtin_func-__obj_trace_push :=
builtin_func-__obj_trace_pop  :=
endif

ifdef OBJ_DEBUG
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
endif

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
#   otherwise it aborts using 'builtin_error'.
define __object_member_parse
	$(or \
		$(__object_member_try_parse),
		$(call builtin_error,
				Invalid first argument to '$(builtin_name)' function: '$1')
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
	$$(foreach __this,$(if $1,
			$$(call __object_check,$1
					$(def-ifdef OBJ_DEBUG,$(\comma)$(subst $$,$$$$,$1))),
			$$(this)),
		$2
	)
endef

$(def_all)

# Params:
#   1. Object reference.
#   2. Original code (if OBJ_DEBUG).
# Return:
#   The trimmed argument if it is a single word, fails with an error otherwise.
define __object_check
	$(or \
		$(suffix $(singleword $1)),
		$(error \
				Invalid object reference: '$1'$(def-ifdef OBJ_DEBUG, ($2)))
	)
endef

#
# Method invocation.
#

#
# $(invoke method,args...)
# $(invoke obj.method,args...)
# $(invoke ref->method,args...)
#
define builtin_func-invoke
	$(call __object_member_parse,$1,$(lambda \
		# 1. Empty for 'this', target object otherwise.
		# 2. Referenced method.
		# 3. Args...
		$(call __object_member_access_wrap,$1,
			$(def-ifdef OBJ_DEBUG,
				$$(foreach __method_name,$2,
					$$(foreach __args_nr,$(words $(builtin_args_list)),
						$$(call __method_invoke_debug,$3)
					)
				),
				$$(call $$($$(__this)).$2,$3)
			)
		)
	),$(builtin_nofirstarg))
endef

ifdef OBJ_DEBUG
# Params:
#   1..N: Args...
# Context:
#   '__class__'
define __method_invoke_debug
	$(foreach __obj_debug_args_nr,$(word $(__args_nr),0 1 2 3 4 5 6 7 8 9),
		$(__obj_trace_push $(__this),invoke $($(__this)).$(__method_name): \
				$(__obj_debug_args))
	)
	$(with \
		$(foreach 0,$(or $(call var_recursive,$($(__this)).$(__method_name)),
				$(error No method '$(__method_name)', \
						invoked on object '$(__this)' of type '$($(__this))')),
			$(expand $(value $0))),
		$(__obj_trace_pop $(__this),invoke $($(__this)).$(__method_name) = '$1')
		$1)
endef
endif

$(def_all)

#
# Property access.
#

#
# $(get [{obj.|ref->}]property)
#
define builtin_func-get
	$(call builtin_check_max_arity,1)
	$(call __object_member_parse,$1,$(lambda \
		# 1. Empty for 'this', target object otherwise.
		# 2. Property.
		$(call __object_member_access_wrap,$1,
			$(def-ifdef OBJ_DEBUG,
				$$(call __property_get_debug,$2),
				$$(call $$($$(__this)).$2.getter)
			)
		)
	))
endef

ifdef OBJ_DEBUG
# Params:
#   1. Property name.
# Context:
#   '__this'
define __property_get_debug
	$(__obj_trace_push $(__this),get    $($(__this)).$1)

	$(with \
		$1,
		$(call $(or $(call var_recursive,$($(__this)).$1.getter),
				$(error \
						No property '$1', performing 'get' \
						on object '$(__this)' of type '$($(__this))'))
		),

		$(__obj_trace_pop $(__this),get    $($(__this)).$1 = '$2')
		$2
	)

endef
endif

#
# $(set  [{obj.|ref->}]property,value)
# $(set+ [{obj.|ref->}]property,value)
# $(set- [{obj.|ref->}]property,value)
#
builtin_func-set  = $(__builtin_func_set)
builtin_func-set+ = $(__builtin_func_set)
builtin_func-set- = $(__builtin_func_set)

# Expanded from  'setx' builtin context. It will generate a call
# to the corresponding property setter, 'x' is taken from builtin name.
define __builtin_func_set
	$(call builtin_check_min_arity,2)
	$(call __object_member_parse,$1,$(lambda \
		# 1. Empty for 'this', target object otherwise.
		# 2. Property.
		# 3. Value being set.
		$(call __object_member_access_wrap,$1,
			$(def-ifdef OBJ_DEBUG,
				$$(call __property_set_debug,$2,$(builtin_name:set%=%),$3),
				$$(call $$($$(__this)).$2.setter$(builtin_name:set%=%),$3)
			)
		)
	),$(builtin_nofirstarg))
endef

ifdef OBJ_DEBUG
# Params:
#   1. Property name.
#   2. '+', '-', or empty.
#   3. Value.
# Context:
#   '__this'
define __property_set_debug
	$(__obj_trace_push $(__this),set$(or $2,$(\s))   $($(__this)).$1: '$3')

	$(call $(or $(call var_recursive,$($(__this)).$1.setter$2),
			$(error No property '$1', performing 'set$2' \
				on object '$(__this)' of type '$($(__this))')),
		$3)

	$(__obj_trace_pop $(__this),set$(or $2,$(\s))   $($(__this)).$1)
endef
endif

#
# Field operations.
#

# Params:
#   1. Field name.
# Context:
#   '__this'
define __field_check
	$(if $(findstring simple,$(flavor $(__this).$1)),
		$1,
		$(error \
				No field '$1', \
				referenced on object '$(__this)' of type '$($(__this))')
	)
endef

#
# $(get-field field)
# $(get-field obj.field)
# $(get-field ref->field)
#
define builtin_func-get-field
	$(call builtin_check_max_arity,1)
	$(call __object_member_parse,$1,$(lambda \
		# 1. Empty for 'this', target object otherwise.
		# 2. Referenced field.
		$(def-ifdef OBJ_DEBUG,
			$(call __object_member_access_wrap,$1,
				$$(call __field_get_debug,$2)),
			$$($(or $1,$$(this)).$2)
		)
	))
endef


ifdef OBJ_DEBUG
# Params:
#   1. Field name.
# Context:
#   '__this'
define __field_get_debug
	$(__obj_trace $(__this),f-get  $($(__this)).$1 = \
		'$($(__this).$(call __field_check,$1))')
	$($(__this).$(call __field_check,$1))
endef
endif

#
# $(set-field  [{obj.|ref->}]field,value)
# $(set-field+ [{obj.|ref->}]field,value)
# $(set-field- [{obj.|ref->}]field,value)
#
builtin_func-set-field  = $(__builtin_func_set_field)
builtin_func-set-field+ = $(__builtin_func_set_field)
builtin_func-set-field- = $(__builtin_func_set_field)

# Expanded from  'set-fieldx' builtin context. It will generate a call
# to '__field_setx', where 'x' is taken from builtin name.
define __builtin_func_set_field
	$(call builtin_check_min_arity,2)
	$(call __object_member_parse,$1,$(lambda \
		# 1. Empty for 'this', target object otherwise.
		# 2. Referenced field.
		# 3. Value.
		$(call __object_member_access_wrap,$1,
			$$(call __field_set$(builtin_name:set-field%=%),$2,$3)
		)
	),$(builtin_nofirstarg))
endef

# Params:
#   1. Field name.
#   2. Value.
# Context:
#   '__this'
define __field_set
	$(__obj_trace $(__this),f-set  $($(__this)).$1: '$2')

	${eval \
		override $(__this).$(__field_check) := $$2
	}
endef

# Params:
#   1. Field name.
#   2. Value.
# Context:
#   '__this'
define __field_set+
	$(__obj_trace $(__this),f-set+ $($(__this)).$1: '$2')

	${eval \
		override $(__this).$(__field_check) += $$2
	}
endef

# Params:
#   1. Field name.
#   2. Value.
# Context:
#   '__this'
define __field_set-
	$(__obj_trace $(__this),f-set- $($(__this)).$1: '$2')

	${eval \
		override $(__this).$(__field_check) := \
			$$(filter-out $$2,$$($(__this).$1))
	}
endef

$(def_all)

#
# Object/class structure introspection.
#

# Tells whether the argument is a valid object reference.
#   1. Reference to check.
# Return:
#   The argument if true, empty otherwise.
define is_object
	$(if $(value class-$(value $(suffix $1))),$1)
endef
builtin_func-is-object = \
	$(foreach builtin_name,is_object,$(builtin_to_function_inline))

# Tells whether a given class has been defined.
#   1. Class name.
# Return:
#   The argument if true, empty otherwise.
define class_exists
	$(if $(value class-$1),$1)
endef
builtin_func-class-exists = \
	$(foreach builtin_name,class_exists,$(builtin_to_function_inline))

# Gets the class of the specified object.
#   1. Object to inspect.
# Return:
#   The class if the argument is a valid object, empty otherwise.
define class
	$(foreach c,$(singleword $(value $(suffix $1))),
			$(if $(value class-$c),$c))
endef
$(call def,class)
builtin_func-class = $(builtin_to_function_inline)

# Tells whether a given object is an instance of the specified class.
#   1. Reference to check.
#   2. Class name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define instance_of
	#$(foreach c,$(class $1),
	#		$(if $(filter $2,$c $($c.super)),$1))

	# Optimized.
	$(foreach c,$(singleword $(value $(suffix $1))),
			$(and $(value class-$c),$(filter $2,$c $($c.supers)),$1))
endef
builtin_func-instance-of = \
	$(foreach builtin_name,instance_of,$(builtin_to_function_inline))

# Tells whether a given object has the specified field.
#   1. Reference to check.
#   2. Field name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define has_field
	$(if $(call class_has_field,$(class $1),$2),$1)
endef
builtin_func-has-field = \
	$(foreach builtin_name,has_field,$(builtin_to_function_inline))

# Tells whether a class has the given field.
#   1. Name of the class to check.
#   2. Field name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define class_has_field
	$(if $(filter $2,$(value $1.fields)),$1)
endef
builtin_func-class-has-field = \
	$(foreach builtin_name,class_has_field,$(builtin_to_function_inline))

# Tells whether a given object has the specified property.
#   1. Reference to check.
#   2. Property name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define has_property
	$(if $(call class_has_property,$(class $1),$2),$1)
endef
builtin_func-has-property = \
	$(foreach builtin_name,has_property,$(builtin_to_function_inline))

# Tells whether a class has the given property.
#   1. Name of the class to check.
#   2. Property name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define class_has_property
	$(if $(filter $2,$(value $1.properties)),$1)
endef
builtin_func-class-has-property = \
	$(foreach builtin_name,class_has_property,$(builtin_to_function_inline))

# Tells whether a given object has the specified method.
#   1. Reference to check.
#   2. Method name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define has_method
	$(if $(call class_has_method,$(class $1),$2),$1)
endef
builtin_func-has-method = \
	$(foreach builtin_name,has_method,$(builtin_to_function_inline))

# Tells whether a class has the given method.
#   1. Name of the class to check.
#   2. Method name.
# Return:
#   The first argument if the answer is true, empty otherwise.
define class_has_method
	$(if $(filter $2,$(value $1.methods)),$1)
endef
builtin_func-class-has-method = \
	$(foreach builtin_name,class_has_method,$(builtin_to_function_inline))

$(def_all)

#
# Class definition.
#

define __class_variable_value_provider
	$(assert $(filter class-%,$1))

	$(if $(not $(call __class_name_check,$(1:class-%=%))),
		$(error \
				Illegal class name: '$(1:class-%=%)')
	)

	$(if $(call def_is_done,$1),
		# Workaround for repeated def in case when the class has already been
		# defined from 'super' builtin of another one.
		$(value $1),
		$$(__class__ \
			$(value $1)$(\n)# \n protects us from a comment on the last line.
		)
	)
endef
$(call def,__class_variable_value_provider)
$(call def_register_value_provider,class-%,__class_variable_value_provider)

#
# $(__class__ fields/methods/supers...)
#
define builtin_macro-__class__
	$(call builtin_check_max_arity,1)

	$(foreach __class__,$(__def_var:class-%=%),
		$(call var_assign_simple,$(__class__),)

		$(expand $1)$$(\0)# XXX

		# Define four special variables needed for introspection.
		# TODO Backward-compatibility, should be replaced
		# by '__class_attr_xxx' API.
		$(call var_assign_simple,$(__class__).supers,
			$(notdir $(call __class_attr_query,super,%)))
		$(call var_assign_simple,$(__class__).methods,
			$(notdir $(call __class_attr_query,method%,%)))
		$(call var_assign_simple,$(__class__).properties,
			$(call __member_name,$(call __class_attr_query,property,%)))

		# Special subsets of fields (for faster traversing/serialization).
		$(with $(notdir $(call __class_attr_query,field,%)),

			$(call var_assign_simple,$(__class__).fields,
				$(call __member_name,$1))

			# List of references: 'name[].type'
			$(call var_assign_simple,$(__class__).reference_list_fields,
				$(call __member_name,$(for f <- $1,
						$(if $(findstring [].,$f),$f))))

			# Single reference: 'name.type'
			$(call var_assign_simple,$(__class__).reference_scalar_fields,
				$(call __member_name,$(for f <- $1,
						$(and $(not $(findstring [],$f)),$(suffix $f),$f))))

			# Plain list: 'name[]'
			$(call var_assign_simple,$(__class__).raw_list_fields,
				$(call __member_name,$(for f <- $1,
						$(and $(findstring [],$f),$(not $(suffix $f)),$f))))

			# Raw value: 'name'
			$(call var_assign_simple,$(__class__).raw_scalar_fields,
				$(filter-out \
					$($(__class__).reference_list_fields) \
					$($(__class__).reference_scalar_fields) \
					$($(__class__).raw_list_fields),
						$(call __member_name,$1)))
		)

	)

endef

# Params:
#   1. Class or member name to check.
# Returns:
#   The argument if it is a valid name, empty otherwise.
define __class_name_check
	$(if $(not $(or \
			$(filter property setter getter field method super class,$1),
			$(findstring $(\\),$1),
			$(findstring $(\h),$1),
			$(findstring $$,$1),
			$(findstring  %,$1),
			$(findstring  .,$1),
			$(findstring  /,$1)
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

# Params:
#   1. Type.
#   2. Name.
define __class_new_attr
	$(assert $(not $(findstring /,$1$2)))

	$(if $(__class_attr_query_self),
		$(call builtin_error,
			Redefinition of '$2' $1)
	)

	$(call var_assign_simple_append,
		$(__class__),$1/$2)
endef

# Params:
#   1. Type.
#   2. Name.
#   3. Body.
define __class_new_attr_func
	$(call __class_new_attr,$1,$2)
	$(call __class_new_func,$2,$3)
endef

# Params:
#   1. Name.
#   2. Body.
define __class_new_func
	$(foreach f,$(__class__).$1,
		$(if $(call var_undefined,$f),
			$(call def_exclude,$f))
		$(call var_assign_recursive_sl,$f,$2)
	)
endef

# Params:
#   1. Name.
#   2. Body.
define __class_new_func_weak
	$(foreach f,$(__class__).$1,
		$(if $(call var_undefined,$f),
			$(call def_exclude,$f)
			$(call var_assign_recursive_sl,$f,$2))
	)
endef

# Searches the current class and its super-type hierarchy (at the moment
# of call, of course) for members matching both of the specified patterns.
# Duplicates are removed.
#
# Params:
#   1. Type patterns.
#   2. Name patterns.
# Return:
#   Quering result in form 'type/name'.
__class_attr_query = $(sort $(__class_attr_query_nosort))
__class_attr_query_nosort = \
	$(__class_attr_query_self_nosort) $(__class_attr_query_supers_nosort)

# Queries for those members that have been defined in the class itself.
# See '__class_attr_query'.
__class_attr_query_self = $(sort $(__class_attr_query_self_nosort))
define __class_attr_query_self_nosort
	$(foreach __class__,$(__class__),# Just a little optimization.
		$(__class_attr_do_query)
	)
endef

# Queries for members defined in supers of the current class.
# See '__class_attr_query'.
__class_attr_query_supers = $(sort $(__class_attr_query_supers_nosort))
define __class_attr_query_supers_nosort
	$(foreach __class__,# Get list of super classes.
		$(sort $(call $(lambda \
			$(foreach __class__,$(notdir $(call __class_attr_do_query,super,%)),
				$(__class__) $(call $0))
		))),
		$(__class_attr_do_query)
	)
endef

# Internal method for retrieving members of the class defined by '__class__'.
# Result may include duplicates.
define __class_attr_do_query
	$(with $1,$(filter $2,$(notdir $($(__class__)))),
		$(filter $(foreach 1,$1,$(addprefix $1/,$2)),$($(__class__))))
endef

# Searches the specified class and its super-type hierarchy.
#   1. Target class.
#   2. Type patterns.
#   3. Name patterns.
__class_attr_query_in = \
	$(foreach __class__,$1,$(call __class_attr_query,$2,$3))

# See '__class_attr_query_in', '__class_attr_query_self'.
__class_attr_query_self_in = \
	$(foreach __class__,$1,$(call __class_attr_query_self,$2,$3))

# See '__class_attr_query_in', '__class_attr_query_supers'.
__class_attr_query_supers_in = \
	$(foreach __class__,$1,$(call __class_attr_query_supers,$2,$3))

#
# $(super ancestor,args...)
#
define builtin_func-super
	$(foreach s,class-$(call __class_resolve,$1),
		$(if $(or \
				$(call def_in_progress,$s),
				$(and \
					$(call def_is_done,$s),
					$(call __class_attr_query_in,$1,super,$(__class__))
				)),
			$(call builtin_error,
				Class '$(__class__)' can't extend '$1' \
				because of inheritance loop)
		)

		$(if $(not $(call def_is_done,$s)),$(call def,$s))

		# Emit a call to a super constructor.
		$(if $(multiword $(builtin_args_list)),
			$$(call $s,$(builtin_nofirstarg)),
			$$(call $s)
		)
	)

	# Must add it after querying the parent for its supers
	# because '__class_attr_query' assumes there is no inheritance loops.
	$(call __class_new_attr,super,$1)

	# Copy function table from super class, but not override functions, that
	# have already been defined in the current class - they always take
	# a precedence over inherited ones. XXX That's not so for now. -- Eldar
	$(silent-for f <- $(call __member_name,
				$(call __class_attr_query_in,$1,method method_stub,%)),
		# XXX There used to be a call to '__class_new_func_weak'... -- Eldar
		$(call __class_new_func,$f,$(value $1.$f)))

	$(silent-for t <- getter setter setter+ setter-,
		f <- $(call __member_name,
				$(call __class_attr_query_in,$1,$t $t_stub,%)),
		$(call __class_new_func,$f.$t,$(value $1.$f.$t)))
endef

# Converts internal member representation ('flavor/name[].type')
# into a plain 'name'. All components except the name itself are optional,
# e.g. 'name.type' or 'flavor/name[]' are both OK.
__member_name = \
	$(notdir $(basename $(subst [],,$1)))

# Params:
#   1. Field name to parse in form 'name[...][:{*|type}]'.
#   2. Continuation with the following args:
#       1. Recognized name, may be with trailing '...'.
#       2. '...', if any.
#       3. Type if specified, empty otherwise.
#       4. Optional argument.
#   3. (optional) Argument to pass to the continuaion.
# Return:
#   Result of call to continuation in case of a valid reference,
#   otherwise it aborts using 'builtin_error'.
define __member_name_parse
	$(or \
		$(with \
			$(subst :, : ,$1),# Split the argument.
			$2,$(value 3),# Continuation function with its argument.

			$(foreach first,$(firstword $1),$(foreach name,
				$(call __class_name_check,$(patsubst %...,%,$(first))),

				$(if $(singleword $1),
					# No type is specified.
					$(call $2,$(name),$(filter-patsubst %...,...,$(first)),,$3),

					# Expecting to see a type in the third word.
					$(if $(eq :,$(word 2,$1)),
						$(foreach type,$(call __class_name_check,$(word 3,$1)),
							$(call $2,$(name),
								$(filter-patsubst %...,...,$(first)),$(type),$3)
						)
					)
				)
			))
		),

		$(call builtin_error,
				Invalid $(builtin_name) name: '$1'$(\comma) \
				should be 'name' or 'name:type'$(\comma) \
				where name can include trailing '...' \
				and type is a class name or '*')
	)
endef

# Checks that a member being defined does not conflicts with an already
# declared one (if any).
#   1. 'method', 'method_stub', 'property' or 'field'.
#   2. Name.
#   3. Cardinality: '...' or empty.
#   4. Type: '*', class name, or empty.
define __member_check_and_def_attr
	$(foreach m_new,$2$(if $3,[])$(if $4,.$4),
		$(foreach m_old,
			$(with $(1:%_stub=%),$2,
				$(notdir $(call __class_attr_query,
					$1 $1_stub,$2 $2[] $2.% $2[].%)),
				# TODO check it. -- Eldar
				$(assert $(not $(multiword $3)),
					__class_attr_query returned too many for '$2' $1: '$3')
				$3
			),

			$(if $(not $(eq $(m_new),$(m_old))),
				$(call builtin_error,
					Conflicting $1 type/cardinality: \
					'$(subst [],...,$(subst ., : ,$(m_new)))'$(\comma) \
					previously declared as \
					'$(subst [],...,$(subst ., : ,$(m_old)))')
			)
		)

		$(call __class_new_attr,$1,$(m_new))
	)
endef

#
# $(method name,body...)
#
define builtin_func-method
	$(and $(call __member_name_parse,$1,$(lambda \
		# 1. Name.
		# 2. '...', or empty.
		# 3. Type, '*', or empty.
		# 4. $(builtin_nofirstarg)

		$(if $(multiword $(builtin_args_list)),

			# Method with an implementation.
			$(call __member_check_and_def_attr,method,$1,$2,$3)
			$(call __class_new_func,$1,
				$$(foreach this,$$(__this),$4)),

			# Abstract method, define a stub.
			$(call __member_check_and_def_attr,method_stub,$1,$2,$3)
			$(call __class_new_func,$1,
				$$(error Invoking unimplemented abstract method $0, \
						declared in class $(__class__)))

		)

		# Due to current design of '__member_name_parse', we have to return
		# something to indicate that everything is ok.
		42# <- the answer.
	),$(builtin_nofirstarg)),)# <- Suppress the output.
endef

#
# $(getter  property,body...)
# $(setter  property,body...)
# $(setter+ property,body...)
# $(setter- property,body...)
#
builtin_func-getter  = $(__builtin_func_xetter)
builtin_func-setter  = $(__builtin_func_xetter)
builtin_func-setter+ = $(__builtin_func_xetter)
builtin_func-setter- = $(__builtin_func_xetter)

# Expanded from getter/setter builtin context.
# Defines an appropriate method, the actual name is taken from builtin name.
define __builtin_func_xetter
	$(call builtin_check_min_arity,2)

	$(if $(not $(call __class_name_check,$1)),
		$(call builtin_error,
				Illegal name: '$1')
	)

	# Check that the corresponding property has already been declared.
	$(with $1,
		$(call __class_attr_query,property,$1 $1[] $1.% $1[].%),

		$(if $(not $2),
			$(call builtin_error,
				You must declare a '$1' property before defining \
				a $(builtin_name:setter%=setter) for it)
		)
		$(assert $(singleword $2),
			__class_attr_query returned too many for '$1' property: '$2')

		$(and \
			$(filter-patsubst setter%,%,$(builtin_name)),
			$(filter-out %[],$(basename $2)),
			$(call builtin_error,
				Add/remove setters can be defined for list properties only)
		)
	)

	$(call __class_new_attr,$(builtin_name),$1)
	$(call __class_new_func,$1.$(builtin_name),
		$(or \
			$(def-ifdef OBJ_DEBUG,
				$(if $(filter setter%,$(builtin_name)),
					$$(if $$(foreach this,$$(__this),$(builtin_nofirstarg)),
						$$(error \
								Setter for '$(basename $0)' property returned \
								non-empty result))
				)
			),
			$$(foreach this,$$(__this),$(builtin_nofirstarg))
		)
	)

endef

#
# $(property name)
# $(property name : *)
# $(property name : type)
#
# $(property name...)
# $(property name... : *)
# $(property name... : type)
#
define builtin_func-property
	$(call builtin_check_max_arity,1)

	$(and $(call __member_name_parse,$1,$(lambda \
		# 1. Name.
		# 2. '...', or empty.
		# 3. Type, '*', or empty.

		$(call __member_check_and_def_attr,property,$1,$2,$3)

		$(call __class_new_attr,getter_stub,$1)
		$(call __class_new_func,$1.getter,
				$$(call __xetter_noimpl,getter,$$(basename $$0)))

		$(call __class_new_attr,setter_stub,$1)
		$(call __class_new_func,$1.setter,
				$$(call __xetter_noimpl,setter,$$(basename $$0)))

		$(foreach s, + -,
			$(call __class_new_attr,setter$s_stub,$1)
			$(call __class_new_func,$1.setter$s,
				$(if $2,$$(call __setter$s,$$(basename $$0),$$1),
						$$(call __setterx_noimpl,$s,$$(basename $$0))))
		)

		# Due to current design of '__member_name_parse', we have to return
		# something to indicate that everything is ok.
		42# <- the answer.
	)),)# <- Suppress the output.
endef

# 1. 'getter' or 'setter'.
# 2. 'class.property'.
define __xetter_noimpl
	$(error Unimplemented $1 for '$2' property)
endef

# 1. '+' or '-'.
# 2. 'class.property'.
define __setterx_noimpl
	$(error set$1 operation is not available for '$2' property)
endef

# 1. Name of setter function with no suffix.
# 2. Value to add.
define __setter+
	$(foreach this,$(__this),$(call $1.setter,$(call $1.getter) $2))
endef

# 1. Name of setter function with no suffix.
# 2. Value to remove.
define __setter-
	$(foreach this,$(__this),$(call $1.setter,
		$(if $(findstring %,$2),
			$(subst %%,%,
				$(filter-out $(subst %,%%,$2),$(subst %,%%,$(call $1.getter)))),
			$(filter-out $2,$(call $1.getter))
		)
	))
endef

#
# $(field name, initializer...)
# $(field name : *, initializer...)
# $(field name : type, initializer...)
#
# $(field name...)
# $(field name... : *)
# $(field name... : type)
#
define builtin_func-field
	$(call __member_name_parse,$1,__field_define,$(builtin_nofirstarg))
endef

#
# $(property-field name, initializer...)
# $(property-field name : *, initializer...)
# $(property-field name : type, initializer...)
#
# $(property-field name...)
# $(property-field name... : *)
# $(property-field name... : type)
#
define builtin_func-property-field
	$(call __member_name_parse,$1,$(lambda \
		# 1. Name.
		# 2. '...', or empty.
		# 3. Type, '*', or empty.
		# 4. Initializer...

		$(call __member_check_and_def_attr,property,$1,$2,$3)

		$(call __class_new_attr,getter_stub,$1)
		$(call __class_new_func,$1.getter,
				$$($$(__this).$1))

		$(call __class_new_attr,setter_stub,$1)
		$(call __class_new_func,$1.setter,
				$$(call __field_set,$1,$$1))

		$(foreach s, + -,
			$(call __class_new_attr,setter$s_stub,$1)
			$(call __class_new_func,$1.setter$s,
				$(if $2,$$(call __field_set$s,$1,$$1),
						$$(call __setterx_noimpl,$s,$$(basename $$0))))
		)

		# Perform the rest initialization as for regular field.
		$(__field_define)

	),$(builtin_nofirstarg))
endef

# 1. Name.
# 2. '...', or empty.
# 3. Type, '*', or empty.
# 4. Initializer...
define __field_define
	$(call __member_check_and_def_attr,field,$1,$2,$3)

#			$(if $2,
#				# Define a type checking setter if there is a type specified.
#				$(call __class_def_attribute_no_check,methods,set.$1)
#				$(call __member_def,method,set.$1,
#					$(if $(eq *,$2),
#						$$(__field_setter_object_check),
#						$$(foreach 2,$2,$$(__field_setter_type_check))
#					)
#				)
#			)

	# Field initializer.
	$$(eval $$(this).$1 := \
		$(if $(not $(findstring $4x,$(trim $4x))),
				$$$$(\0))# Preserve leading whitespaces.
		$(subst $$,$$$$,$4))
endef

## 1. Value being set.
## 2. Type.
#define __field_setter_type_check
#	$(foreach 1,$1,
#		$(or $(instance-of $1,$2),
#			$(error \
#					Attemp to assign value '$1' ($(if $(is-object $1),
#							instance of class $(class $1),not an object)) \
#					to field '$(subst .method.set.,.,$0)' \
#					of incompatible type '$2')
#		)
#	)
#endef
#
## 1. Value being set.
#define __field_setter_object_check
#	$(foreach 1,$1,
#		$(or $(is-object $1),
#			$(error \
#					Attemp to assign value '$1' which is not a valid object \
#					to field '$(subst .method.set.,.,$0)')
#		)
#	)
#endef

#
# Serialization stuff.
#

# Traverses the object graph starting with given objects and assigning a unique
# identifier to each reacheable object.
#
# Param:
#   1. List of objects acting as graph entry points.
#   2. Optional object serial ID provider to use which should return
#      a singleword unique object identifier starting with a period: '.xxx'
#      Defaults to the identity function.
# Return:
#   List of _newly_ reached (untouched) objects.
#   Once touched an object is not participate in traversing of the graph.
#   This in particular means that subsequent calls on the same object graph
#   will return an empty list. Moreover, even if some already touched object is
#   modified so that some yet untouched objects would become reachable, these
#   objects will not be returned.
#   To avoid such problems, avoid multiple calls to this function.
define object_graph_traverse
	$(if $(multiword $(value 2)),
		$(error Bad identifier provider name: '$2'))

	$(for id_fn <- $(or $(trim $(value 2)),id),
		o <- $(suffix $1),

		$(with $o,$($o),
			$(if $(value $1.__serial_id__),
				# The object has already been visited. Do nothing.
				,# else
				# Return the object marking it with a generated identifier.
				$1 $(call var_assign_simple,$1.__serial_id__,$($(id_fn)))
				$(assert $(and \
						$($1.__serial_id__),
						$(singleword [$($1.__serial_id__)]),
						$(not $(findstring $$,$($1.__serial_id__))),
						$(not $(basename $($1.__serial_id__))),
						$(eq $($1.__serial_id__),
							$(suffix $($1.__serial_id__)))),
					Bad serial identifier: '$($1.__serial_id__)')
				# Recursively process the objects referenced by this one.
				$(for o <- \
					$(for f <-
						$($2.reference_list_fields) \
						$($2.reference_scalar_fields),
							$(suffix $($1.$f))) \
					$(if $(filter __serialize_extra_objects,$($2.methods)),
						$(invoke 1->__serialize_extra_objects)),
					$(call $0,$o,$($o)))
			)
		)
	)
endef

# Results in a Make code sufficient to reproduce the object graph.
# If a serial ID provider is specified in the second argument, then all object
# identifiers and references are properly replaced by the provided values.
#
# Param:
#   See 'object_graph_traverse'.
# Return:
#   Make code. It doesn't include allocation-related stuff,
#   its up to the serial ID provider to track the objects being serialized.
define object_graph_print
	$(subst $(\n) ,$(\n),
		$(for o <- $(object_graph_traverse),
			s <- $($o.__serial_id__),
			c <- $($o),

			$(__object_print)
		)
	)
endef

# Context:
#   o. The object.
#   s. Its serial ID.
#   c. Its class.
define __object_print
	$(\h) $o$(\n)# Comment with an original object identifier.
	$s := $c$(\n)

	$(for ft <- $(for l <- reference_list reference_scalar raw_list raw_scalar,
			$(addsuffix .$l,$($c.$l_fields))),
		f <- $(basename $(ft)),
		# Call field printer with possibly preprocessed field value.
		$(call __object_print_field$(suffix $(ft)),
			$(if $(class-has-method $c,__serialize_field-$f),
				$(invoke o->__serialize_field-$f,$($o.$f)),
				$($o.$f)))
	)

	$(\n)
endef

__object_field_escape = \
	$(subst $(\n),$$(\n),$(subst $(\h),$$(\h),$(subst $$,$$$$,$1)))

define __object_print_field.reference_list
	$s.$f := \
		$(for r <- $1,\$(\n)$(\t)
			$(assert $(is-object $r),
				Not an object '$r' inside reference field $c.$f \
				of object $o being serialized as $s)
			# Substitute the suffix with the serial identifier of the
			# referenced object and escape everything else.
			$(call __object_field_escape,$(basename $r))
			$($(suffix $r).__serial_id__))$(\n)
endef

define __object_print_field.reference_scalar
	$(assert $(not $(multiword $1)),
		Multiword value '$1' inside scalar field $c.$f \
		of object $o being serialized as $s)
	$s.$f := \
		$(for r <- $1,
			$(assert $(is-object $r),
				Not an object '$r' inside reference field $c.$f \
				of object $o being serialized as $s)
			# See '__object_print_field_reference_list'.
			$(call __object_field_escape,$(basename $r))
			$($(suffix $r).__serial_id__))$(\n)
endef

define __object_print_field.raw_list
	$s.$f := \
		# Guard a trailing backslash (if any)
		# and pretty-print each list item on a separate line.
		$(patsubst %,\$(\n)$(\t)%,$(subst-end \,$$(\\),
			$(__object_field_escape)))$(\n)
endef

define __object_print_field.raw_scalar
	$s.$f := \
		# Check for a leading whitespace.
		$(if $(subst x$(firstword $1),,$(firstword x$1)),
			$$(\0))
		# Guard a trailing backslash (if any).
		$(subst \$(\n),$$(\\)$(\n),
			$(__object_field_escape)$(\n))
endef

$(def_all)

endif # __core_object_mk
