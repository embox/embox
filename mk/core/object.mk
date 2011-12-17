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
#   Field
#     - a value associated with each object of a certain class, provides a
#       runtime state of the instance.
#
#   Reference field
#     - a special kind of field, that hold references to other objects.
#
#   Method
#     - a function associated with a class, defines runtime behavior of an
#       instance of the class. Method is evaluated in a special context with
#      'this' pointing the current object, thus it has as access to other
#       members of the object.
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
#   	$(field name[:{*|type}][,initializer...])
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
#   	$(field name[,implementation...])
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
#   	$(field say_hello,
#   		$(info Hello from '$(this)', the first argument is '$1'))
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
#   	$(get  field)       # Retrieves the current field value.
#
#   	$(set  field,value) # Assigns a new value to the field.
#   	$(set+ field,value) # Appends a given value to the field.
#   	$(set* field,value) # Appends a value if the field doesn't contain it.
#   	$(set- field,value) # Removes any occurrence of a value from the field.
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
#   There is, however, a special property of references: prepending an
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
#   During class definition or a new object instantiation a number of ancillary
#   variables are created.
#
#   Considering the code below as an example, we'll examine what variables
#   are created and when.
#
#   	define class-zuper
#   		$(info Super constructor with argument '$1')
#   	endef
#
#   After defining class 'zuper' variable 'class-zuper' becomes a part
#   of its constructor. Also three special variables defined.
#
#   	# class-zuper = $(info Super constructor with argument '$1')
#
#   	# zuper.super :=
#   	# zuper.fields :=
#   	# zuper.methods :=
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
#   	# clazz.fields := foo
#   	# clazz.methods := bar
#
#   	# clazz.foo = \
#   	# 	$(info Initializing field 'foo')initial value
#   	# clazz.bar = \
#   	# 	$(foreach this,$(__this),$(info Calling method 'bar')hello world)
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
#   	# .obj1.foo := initial value
#
#

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

# Creates a new inctance of the specified class.
# Only a single argument is passed to the constructor.
#   1. Class name.
#   2. An (optional) argument to pass to the constructor.
# Return:
#   Newly created object instance.
define new
	$(new $1,$(value 2))
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
	# It is mandatory for object references to start with a period.
	$(foreach this,.obj$(words $(__object_instance_cnt) x),
		$(def-ifdef OBJ_DEBUG,
			$(info \
					$(this): new    $(__class__): $(__obj_debug_args))
		)
		${eval \
			__object_instance_cnt += $(this:.obj%=%)
			$(\n)
			$(this) := $(__class__)
			$(\n)
			$$(and $(value class-$(__class__)),)
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
	$(foreach __obj_debug_args_nr,$(word $(__args_nr),0 1 2 3 4 5 6 7 8 9),
		$(info \
				$(__this): invoke $($(__this)).$($(__args_nr)): \
				$(__obj_debug_args))
	)
	$(foreach 0,$(or $(call var_recursive,$($(__this)).$($(__args_nr))),
			$(error \
					No method '$($(__args_nr))', \
					invoked on object '$(__this)' of type '$($(__this))')),
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
				No field '$1', \
				referenced on object '$(__this)' of type '$($(__this))')
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
	$(def-ifdef OBJ_DEBUG,$(info $(__this): set    $1.$2: '$3'))
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
	$(def-ifdef OBJ_DEBUG,$(info $(__this): set+   $1.$2: '$3'))
	${eval \
		$(if $($(__this).$(call __field_check,$2)),
			$(if $(value $1.set.$2),
				override $(__this).$2 := \
					$$(call $1.set.$2,$$($(__this).$2) $$3)
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
# $(set* field,value)
# $(set* obj.field,value)
# $(set* ref->field,value)
#
define builtin_func-set*
	$(__builtin_func_set)
endef

# Params:
#   1. Class.
#   2. Field name.
#   3. Field value.
# Context:
#   '__this'
define __field_set*
	$(def-ifdef OBJ_DEBUG,$(info $(__this): set*   $1.$2: '$3'))
	$(if $(findstring $(\s)$3 , $($(__this).$(call __field_check,$2)) ),
		,# else
		$(call __field_set+,$1,$2,$3)
	)
endef

#
# $(set- field,value)
# $(set- obj.field,value)
# $(set- ref->field,value)
#
define builtin_func-set-
	$(__builtin_func_set)
endef

# Params:
#   1. Class.
#   2. Field name.
#   3. Field value.
# Context:
#   '__this'
define __field_set-
	$(def-ifdef OBJ_DEBUG,$(info $(__this): set-   $1.$2: '$3'))
	$(call __field_set,$1,$2,$(trim $(subst $(\s)$3 , , $($(__this).$2) )))
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
				$$(call __field_get,$$($$(__this)),$2)
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
			$(and $(value class-$c),$(filter $2,$c $($c.super)),$1))
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
	$(if $(filter $2,$(basename $(value $1.fields))),$1)
endef
builtin_func-class-has-field = \
	$(foreach builtin_name,class_has_field,$(builtin_to_function_inline))

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
				$(findstring  .,$1),
				$(findstring  -,$1),
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
		$(\n)
		__def_ignore += $1.$2
	}
endef

# Params:
#   1. Field name to parse in form 'name' or 'name:type'.
#   2. Continuation with the following args:
#       1. Recognized name.
#       2. Type if specified, empty otherwise.
#       3. Optional argument.
#   3. (optional) Argument to pass to the continuaion.
# Return:
#   Result of call to continuation in case of a valid reference,
#   otherwise it aborts using 'builtin_error'.
define __field_name_parse
	$(or \
		$(with \
			$(subst :, : ,$1),# Split the argument.
			$2,$(value 3),# Continuation function with its argument.

			$(foreach name,$(call __class_name_check,$(firstword $1)),
				$(if $(singleword $1),
					# No type is specified.
					$(call $2,$(name),,$3),

					# Expecting to see a type in the third word.
					$(if $(eq :,$(word 2,$1)),
						$(foreach type,$(call __class_name_check,$(word 3,$1)),
							$(call $2,$(name),$(type),$3)
						)
					)
				)
			)
		),

		$(call builtin_error,
				Invalid field name: '$1'$(\comma) \
				should be 'name'$(\comma) 'name:type'$(\comma) or 'name:*')
	)
endef

#
# $(field name,initializer...)
# $(field name:*,initializer...)
# $(field name:type,initializer...)
#
define builtin_func-field
	$(call __field_name_parse,$1,
		# 1. Name.
		# 2. Type, '*', or empty.
		# 3. Initializer...
		$(lambda $(foreach c,$(call builtin_tag,__class__),
			$(call __class_def_attribute,fields,$1$(if $2,.$2))
			$(call __member_def,$c,$1,$3)

			$(and $2,# Define a setter if there is a type specified.
				$(call var_undefined,$c.set.$1),

				$(call __class_def_attribute_no_check,methods,set.$1)
				$(call __member_def,$c,set.$1,
					$(if $(eq *,$2),
						$$(__field_setter_object_check),
						$$(foreach 2,$2,$$(__field_setter_type_check))
					)
				)
			)

			# Field initializer.
			$$(eval $$(this).$1 := $$(value $c.$1))
		)),
		$(builtin_nofirstarg)
	)
endef

# 1. Value being set.
# 2. Type.
define __field_setter_type_check
	$(foreach 1,$1,
		$(or $(instance-of $1,$2),
			$(error \
					Attemp to assign value '$1' ($(if $(is-object $1),
							instance of class $(class $1),not an object)) \
					to field '$(subst .set.,.,$0)' of incompatible type '$2')
		)
	)
endef

# 1. Value being set.
define __field_setter_object_check
	$(foreach 1,$1,
		$(or $(is-object $1),
			$(error \
					Attemp to assign value '$1' which is not a valid object \
					to field '$(subst .set.,.,$0)')
		)
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
		$(if $(multiword $(builtin_args_list)),
			$$(foreach this,$$(__this),$(builtin_nofirstarg)),
			$$(error Invoking unimplemented abstract method $0, \
					declared in class $(call builtin_tag,__class__))
		)
	)
endef

#
# $(super ancestor,args...)
#
define builtin_func-super
	$(call __class_def_attribute,super,$1)

	$(foreach c,class-$(call __class_resolve,$1),
		$(if $(not $(call def_is_done,$c)),$(call def,$c))

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

define field_name
	$(basename $1)
endef

define field_type
	$(suffix $1)
endef

define graphviz_escape
	$(subst ",\",$(subst |,\|,$1))
endef

define obj_links
	$(subst .,,$(basename $($($1).fields:%=.%)))
endef

$(def_all)

#param 1 node
define get_leaves
	$(foreach f,$(subst .,,$(basename $($($1).fields:%=.%))),
		$(foreach p,$(suffix $($1.$f)),$p)
	)
endef

#param $1 current object
#param $2 current marked object list
define __object_get_list
	$(foreach f,$(or $(singleword $2),$(error invalid argument in $0: '$2')),
		$(sort $(with $1,,
				$(if $(filter $1,$2),$2,
					$(foreach o,$(call $f,$1),
						$(call $0,$o,$2 $1)
					)
				)
			)
		)
	)
endef

define __object_dump_dot
	$(info $(call __object_get_list,.obj7,get_leaves))
	$(\n)digraph "Make Objects Dump"
	$(\n){
	$(\n)	graph[rankdir="LR"];
	$(\n)	node[shape="record"];
	$(\n)
	$(foreach o,$(__object_instance_cnt:%=.obj%),
		$(\n)	"$o" \
			[label="<.> $o : $($o)\l $(foreach f,$(call field_name,$($($o).fields)),
				| <$f> $f = $(call graphviz_escape,$($o.$f))\l
			)"];
		$(\n)
		$(foreach f, $(call obj_links,$o),
			$(foreach p,$(call field_type,$($o.$f)),
				$(\n)	"$o":$f -> "$p":".";
			)
		)
		$(\n)
	)
	$(\n)}
	$(\n)
endef




include mk/util/escape.mk

define objects_to_mk
	$(foreach o,$(call __object_get_list,.obj7,get_leaves),
		$(foreach f,$(call field_name,$($($o).fields)),
			$o:=$(call escape_makefile,$($o))
			$(\n)
			$o.$f:=$(call escape_makefile,$($o.$f))
			$(\n)
		)
	)
	$(\n)
endef

$(def_all)

__mk_objects_dump_ps := objects_dump.ps

.PHONY : mk_objects_dump mk_object_to_mk
.PHONY : $(__mk_objects_dump_ps:.ps=.dot) # Assume it volatile.
mk_objects_dump : $(__mk_objects_dump_ps)

$(__mk_objects_dump_ps:.ps=.dot) :
	@printf '%b' '$(call my_printf_escape,$(__object_dump_dot))' > $@

$(__mk_objects_dump_ps) : %.ps : %.dot
	@dot -Tps $< -o $@

mk_object_to_mk:
	@printf '%b' '$(call escape_printf,$(call objects_to_mk))' > dump.mk

endif # __core_object_mk
