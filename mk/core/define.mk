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
# Make functional language extensions:
#  - Syntactic:
#    - Defining multiline verbose functions with inline comments;
#    - Using tabs for function code indentation.
#  - Semantic:
#    - Comprehensive framework for defining own builtin functions
#      with opportunities to inspect expansion stack, inline calls to certain
#      functions and check usages of native builtins;
#    - 'lambda' and 'with' builtins for defining anonymous inner functions.
#
#   Date: Jun 28, 2011
# Author: Eldar Abusalimov
#

ifndef __core_def_mk
__core_def_mk := 1

##
# Example of typical usage scenario is the following:
# TODO more docs. -- Eldar
#
##
#   include mk/core/def.mk
#
#   define foo
#   	$(call bar,
#   		# Zzz...
#   		baz
#   	)
#   endef
#   $(call def,foo)
#

include mk/core/common.mk
include mk/core/string.mk

include mk/util/list.mk
include mk/util/var/assign.mk
include mk/util/var/info.mk

##
# Performs both syntactiacal and semantical transformations of a function
# defined with a given name.
#
# Params:
#   1. Name of the function(s) being defined.
#   2. (optional) Value interceptor function ('value' by default).
# Return:
#   Nothing.
def = \
	$(and $(foreach __def_var,$1, \
		$(if $(value DEF_TRACE), \
				$(warning $(\s)$(\t)def: \
						$(flavor $(__def_var))$(\t)[$(__def_var)])) \
		$(if $(call var_recursive,$(__def_var)), \
			$(call var_assign_recursive_sl,$(__def_var),$ \
				$(call __def,$(call $(or $(value 2),value),$(__def_var)))), \
			$(if $(call var_undefined,$(__def_var)), \
				$(error Function '$(__def_var)' is not defined) \
			) \
		) \
	),)$(call var+=,__def_done,$1)

##
# Translates all functions defined since the last call.
#
# Return:
#   Nothing.
# Note:
#   The order of translation may differ from the order of definition.
# Note:
#   This does not affects functions defined with 'builtin_aux_def'.
# Note:
#   No need to 'call' it, just expand: $(def_all).
def_all = \
	$(call def,$(filter-out $(__def_done) $(__def_ignore),$(.VARIABLES)))

__def_done   :=
__def_ignore := $(.VARIABLES) __def_ignore

# Params:
#   1. Code of a function being defined.
# Return:
#   Processed code ready to replace the original value of the function.
__def = \
	$(call __def_builtin,$ \
		$(call __def_brace,$ \
			$(call __def_strip,$ \
				$(subst $$,$$$$,$1))))

# Params:
#   1. Code with dollars escaped.
# Return:
#   The code with comments, newlines and indentation discarded.
#   Dollars remain escaped.
__def_strip = \
	$(__def_strip_precheck)$ \
	$(call __def_strip_unescape,$ \
		$(call list_scan,__def_strip_fold,_$$n,$ \
			$(call __def_strip_escape,$ \
				$1)))

# Params:
#   1. Code with dollars escaped.
# Return:
#   Nothing.
__def_strip_precheck = \
	$(if $(findstring \$(\s),$(subst $(\t),$(\s),$(subst \\,,$1))),$ \
		$(warning Backslash followed by a whitespace \
				is probably not what you want)$ \
	)

# Params:
#   1. Code with dollars escaped.
# Return:
#   The code with whitespaces and some other control chars replaced with
#   special markers (for instance, \n -> _$$n).
__def_strip_escape = \
  $(subst _$$l,\,$        \
   $(subst \ _$$h ,$(\h),$ \
    $(subst \\,_$$l,$       \
     $(subst  $(\h), _$$h ,$ \
      $(subst  $(\n), _$$n ,$ \
       $(subst  $(\t), _$$t ,$ \
        $(subst  $(\s), _$$s ,$ \
         $1)))))))

# Code convolution function.
# Params:
#   1. Previous token or empty inside a comment.
#   2. Current token.
# Return:
#   Token to append to the resulting text.
__def_strip_fold = \
  $(and $(subst _$$h,,$2), \
        $(if $(findstring _$$n,$1),$ \
             $(subst _$$s,_$$n,$(subst _$$t,_$$n,$2)),$ \
             $(or $(findstring _$$n,$2), \
                  $(if $1,$2))) \
   )

# Params:
#   1. Code with control characters inserted by '__def_strip_escape'.
# Return:
#   The code with restored whitespaces (space and tabs).
#   Newlines are discarded.
__def_strip_unescape = \
  $(subst  _$$s,$(\s),$ \
   $(subst  _$$t,$(\t),$ \
    $(subst  _$$n,,$      \
     $(subst $(\s),,$      \
      $1))))

#
# Starting at this point the rest functions will be written using new syntax
# that we have just defined.
# But since '__def_brace' and '__def_builtin' functions have not been
# implemented yet, we need to define stubs conforming to their interfaces.
#

# See '__def_brace_real'.
__def_brace = \
	$(subst $$$$,$$,$1)

# See: '__def_builtin_real'.
__def_builtin = \
	$1

#
# Now we can use $(call def,...) and $(def_all).
#
$(def_all)

# Expands value of the argument treating it as a code. The code is a single
# line string with no leading whitespaces.
# Params:
#   1. Make code to be expanded.
# Return:
#   Expansion result.
# Note:
#   As a side effect all escaped dollars become unescaped.
define __def_expand
	# It is essential to use simple variable definition here to allow
	# redifinitions inside the expansion.
	#
	# The reason of this limitation is a bug in expansion engine of
	# GNU Make which affects all recent versions that implement 'eval'
	# function (any of 3.8x).
	#
	# The simplest case that reproduces this bug is expanding recursive
	# variable 'foo' defined as follows:
	#
	#   foo = $(eval foo =)
	#
	# In that case steps that Make performs to expand string $(foo) are:
	#   1. Start expanding a reference to a recursive variable 'foo':
	#        $(foo)
	#   2. Get its value and start parsing it:
	#        $(eval foo =)
	#   3. Find an invocation of builtin function 'eval' and run it:
	#        foo =
	#   4. Redefine the variable 'foo' which is being expanded right now.
	#      Redefinition of an already defined variable also involves
	#      freeing a memory occupied by an old value.
	#   5. Return from builtin function handler back to string expansion
	#      code (see step 2).
	#   6. At this point the string being parsed refers to a memory block
	#      that has been freed in step 4.
	#
	# Symptoms that one can observe may vary depending on the actual value
	# of an expanding variable. These may include:
	#   - 'missing separator' error,
	#   - 'unterminated variable reference' and 'unterminated call to function'
	#      errors,
	#   - garbage in results in case if expansion succeeds.
	# The latter is clearly seen when running Make under Valgrind with
	# --free-fill flag.
	#
	# A possible workaround is to use simple variable assignments, like:
	#
	#   foo := $(eval foo =)
	#
	# Here expansion of the value being assigned occurs immediately before
	# actually assigning it and no redefinitions of recursive variables is
	# performed.
	#
	# The most significant drawback is that we can't use 'define' directive
	# in version 3.81 and lower because such variable would be defined
	# recursive. This forces us to escape newlines and comment hashes
	# properly.
	${eval \
		# Use immediate expansion to allow recursive invocations of 'def' and
		# reuse '__def_tmp__' variable (e.g. in '__builtin_args_expand').
		__def_tmp__ := \
			# Prevent interpreting hashes as comments.
			$(subst $(\h),$$(\h),
				# We don't care about any trailing backslash here because
				# Make actually gets EOF at the end of line, not a newline.
				$1
			)
	}
	$(__def_tmp__)
endef

# Substitutes all unescaped occurrences of ${...} expansion to $(...).
# This does not affect pure {...} groups or escaped $${...}.
# Params:
#   1. Dollar-escaped value.
# Return:
#   The unescaped result of brace substitution.
define __def_brace_real
	# Expand hooks.
	$(call __def_expand,
		# Replace true ${...} expansion occurrences to a hook call:
		#   ${call __def_brace_hook,...}
		# which will echo its argument surrounded by $(...)
		$(subst $$$${,$${call __def_brace_hook$(\comma),
			# Isolate doubled dollars (in terms of the original value)
			# from the succeeding text.
			#   $${...}  -> $($)$($){...}
			#   $$${...} -> $($)$($)${...}
			$(subst $$$$$$$$,$$($$$$)$$($$$$),
				# First of all, replace commas with references to \comma.
				# This is needed to get properly handling of cases like
				#   $(call foo,${xxx bar,baz})
				# that would otherwise result in an error:
				#   unterminated call to function `xxx': missing `}'
				$(subst $(\comma),$$(\comma),
					$1
				)
			)
		)
	)
endef

# Surrounds the argument with '$(' and ')'.
__def_brace_hook = \
	$$($1)

# Brace-to-paren substitution logic is now ready.
# Flush before replacing '__def_brace' and diving deeper.
$(def_all)

__def_brace = \
	$(if $(findstring {,$1),$(call __def_brace_real,$1),$(subst $$$$,$$,$1))

#
# Here goes builtin functions transformation stuff.
#
# This part of logic is probably the most tricky and complicated.
# The processing is performed in two phases: inner and outer. Each phase
# involves some code transformation (namely installation of so-called hooks)
# with subsequent expansion of the code which fires the installed hooks.
#
# Inner phase is focused on structural checks of code. Depending on what it
# does recognize (variable expansion, substitution reference, function
# call, etc.) it installs necessary hooks for the outer phase.
#
# Outer phase manages a so-called expansion stack which is used to track how
# the expansion is actually performed by Make. It also helps to collect
# arguments passed to functions occurred in the code. Unlike inner the outer
# phase performs high-level code checks and transformations which include
# checking number of arguments passed to a certain function, defining extended
# builtin functions and so on.
#
# In order to prevent execution of the code being processed all dollar signs
# are quadruplicated ($ -> $$$$). After both expansions have been performed
# dollars are automatically restored to the initial state ($$$$ -> $$ -> $).
#

# The real work is done here.
# 1. Text
define __def_builtin_real
	$(call __def_expand,$(call __def_inner_unescape,
		$(call __def_expand,$(call __def_inner_install_hooks,
			# Escape dollars twice.
			$(subst $$,$$$$$$$$,
				# '__def_root__' echoes everything passed to it.
				$$(__def_root__ $1)
			)
		))
	))
endef

define __def_inner_install_hooks
	$(subst $$$$$$$$$[,
			$$$[call __def_inner_hook$(\comma),
		# Doubly escaped double dollars. I am a rich man. $)
		$(subst $$$$$$$$$$$$$$$$,
			# The hell below is '$($$)$($$)' with dollars and parens escaped.
			$$$$_$$$$[$$$$$$$$_$$$$]
			$$$$_$$$$[$$$$$$$$_$$$$],
			$(subst $(\comma),_$$$$c,
				$1
			)
		)
	)
endef

define __def_inner_escape
	$(subst $[,_$$[,$(subst $],_$$],
		$(subst :,_$$l,$(subst _$$q,=,
			$(subst $(\s),_$$s,$(subst $(\t),_$$t,
				$1
			))
		))
	))
endef

define __def_inner_unescape
	$(subst _$$[,$[,$(subst _$$],$],
		$(subst _$$l,:,$(subst _$$q,=,
			$(subst _$$s,$(\s),$(subst _$$t,$(\t),
				$1
			))
		))
	))
endef

define __def_inner_hook
	$(call __def_inner_escape,
		$(or \
			$(foreach first,$(firstword $1),
				# Guaranteed non empty value inside $(...).
				$(__def_inner_handle)
			),
			# Empty variable name or nothing except whitespaces.
			$$(call __def_outer_hook_warning,
				$$$$($1),
				Empty variable name
			)
		)
	)
endef

#
# Params:
#   1. Non-empty value being handled.
#   'first': The first word of the value.
#
# Return:
#   Resulting handled value with (possibly) outer hooks installed if the value
#   is syntactically correct, or empty otherwise.
#
# Invariants:
#   Everything that has been already handled before is mangled so that
#   there is no whitespaces, escaped commas, colons or equal signs in such
#   handled fragments.
#
#   For example, if we are going to handle the outermost expansion of
#     $(foo bar$(one:%r=%z),baz$(two bob,alice))
#   then the actual value being handled would have a form of
#     $(foo bar???,baz???)
#
#   This avoids handling values more than once and prevents interference with
#   the processing of the current values. Referring to the example above, the
#   value will have only two words (unlike three words in the original
#   expansion), only one comma (as opposed to two commas) and no equal sign or
#   colon.
define __def_inner_handle
	$(or \

		# First try to treat it as a variable expansion with a substitution
		# reference.
		$(__def_inner_handle_substitution_reference),

		# Check that there is no commas in function or variable name.
		$(if $(findstring _$$c,$(first)),
			# Invalid name. Emit a warning hook.
			#   $(foo,bar) $(foo, bar) $(foo,) $(,foo) $(,)
			$$(call __def_outer_hook_warning,
				$$$$($(subst _$$c,$(\comma),$1)),
				Unexpected '$$(\comma)' in variable or function name
			)
		),

		# No commas in the first word. Assuming that it is the only word inside
		# the value being expanded and there is no whitespaces around it
		# we try to handle it as a regular variable expansion.
		#   $(foo)
		$(call singleword,$$$$($1)),

		# Falling in here means that there are some whitespaces inside
		# the expansion parens, either between chars or around them...

		# Check if the first char is not a whitespace.
		$(if $(filter-out x$(first),$(firstword x$1)),
			# There are some, name is bad:
			#   $( foo) $( foo bar)
			$$(call __def_outer_hook_warning,
				$$$$($(subst _$$c,$(\comma),$1)),
				Unexpected leading whitespace in variable or function name
			)
		),

		# No leading whitespaces: valid function call.
		$(__def_inner_handle_function)
	)
endef

# Params:
#   The same as to '__def_inner_handle'.
# Return:
#   The value inside expansion parens if it is a valid substitution reference,
#   or empty otherwise.
define __def_inner_handle_substitution_reference
	$(and \

		# If there is a colon in the first word...
		$(findstring :,$(first)),

		# ...and there is an equal sign after the colon...
		$(findstring =,$(call nofirstword,$(subst :,: ,$1))),

		# ...then it seems to be a substitution reference. Substitutions
		# are not our friends. Stop handle here and output the value as is.
		$$$$($1)
	)
endef

# Params:
#   The same as to '__def_inner_handle'.
# Return:
#   Resulting value with necessary outer hooks installed in case that it is
#   a valid function call, or empty otherwise.
define __def_inner_handle_function
	$(if $(findstring undefined,$(flavor builtin_tag-$(first))),

		# Plain push and handle.
		$$(call __def_outer_hook_push,$(first))
		$(__def_inner_handle_function_pushed),

		# Handle only if the push handler resulted in a non-empty tag.
		$$(foreach __def_outer_tag_$(first),
			$$(call __def_outer_hook_push_tag,$(first)),
			$(__def_inner_handle_function_pushed)
		)
	)
endef

# See '__def_inner_handle_function'
define __def_inner_handle_function_pushed
	$$(call __def_outer_hook_func

		# Unescape any escaped commas and install argument hooks
		# after each of them.
		$(subst _$$c,
			# Real commas are needed to get actual arguments placed
			# into the corresponding variables ($1,$2,...), and
			# hooks help to get the list of theese variables.
			$(\comma)$$(call __def_outer_hook_arg),

			_$$c# Escaped comma before the actual arguments (if any).

			# The one and only word with trailing whitespace is anyway a valid
			# function call with a single empty argument: '$(foo )',
			# and in such case we've done.

			# Otherwise (one non-empty or more than one argument),
			# we should deal with these arguments.
			#   $(foo bar) $(foo ,) $(foo bar,) $(foo ,bar) ...
			$(if $(word 2,$1),
				# The actual arguments form the rest of the value being
				# handled. An exact structure of whitespaces between the
				# function name and the first argument is not meaningful and
				# thus it is not preserved. Any trailing whitespaces are
				# guarded with $(\0) from being stripped out.
				$(subst $[,
					_$$[
					$$_$$[call __def_outer_hook_push$(\comma)__paren___$$],
					$(subst $],
						$$_$$[__def_outer_hook_pop_$$]
						_$$],
						$(call nofirstword,$1$$_$$[\0_$$])
					)
				)
			)
		)
	)
endef

#
# The main structure used by the outer expansion phase is an expansion stack.
# The stack holds an information about how native make expansion engine
# handles a value prepared by inner phase.
# Each element of the stack corresponds to a function call or an expression
# inside parens.
#

# The stack element is represented by a single word in form:
#   foo,1,2 bar,1 __def_root__,1
# where 'foo' and 'bar' are names of the corresponding functions,
# and comma separated numbers refer to function arguments that have already
# been processed.
__def_stack     :=# Initially empty.

# Top of the stack is separated from the rest elements. The only difference is
# that the funсtion name and numbers are separated using spaces:
#   boo 1 2 3
__def_stack_top :=# Empty too.

# Params:
#   1. Function name.
define __def_stack_push_mk
  # Save the current value of the top into the stack.
  __def_stack := \
    $(subst $(\s),$(\comma),$(__def_stack_top)) \
    $(__def_stack)
  # Put new function name onto the top.
  __def_stack_top := $1
endef
__def_stack_push_mk := $(value __def_stack_push_mk)

define __def_stack_arg_mk
  # Append the next argument number to the stack top.
  __def_stack_top += \
    $(words $(__def_stack_top))
endef
__def_stack_arg_mk := $(value __def_stack_arg_mk)

define __def_stack_pop_mk
  # Restore the top from the stack.
  __def_stack_top := \
    $(subst $(\comma),$(\s),$(firstword $(__def_stack)))
  # And remove from the stack.
  __def_stack := \
    $(call nofirstword,$(__def_stack))
endef
__def_stack_pop_mk := $(value __def_stack_pop_mk)

# Debugging stuff.
ifdef DEF_DEBUG
# 1. Msg.
__def_debug = \
	$(warning def($(__def_var)): \
 		$(if $(__def_stack_top),$(\t))$(__def_stack:%=>$(\t))$1)
else
__def_debug :=
endif # DEF_DEBUG

# Pushes the specified name onto the top of the expansion stack saving the
# previous value of the top into the stack.
# Params:
#   1. Value to push.
# Return:
#   Nothing.
define __def_outer_hook_push
	$(call __def_debug,push [$1])
	${eval \
		$(__def_stack_push_mk)
	}
endef

# Params:
#   1. Value to push.
# Return:
#   A singleword tag or empty to omit the whole subexpression.
define __def_outer_hook_push_tag
	$(__def_outer_hook_push)
	$(or \
		$(call __def_outer_tag_check,$(call builtin_tag-$1)),
		$(__def_outer_hook_pop)
	)
endef

# Params:
#   1. Return value of the push handler.
# Return:
#   The argument if is ok, no return (die) otherwise.
define __def_outer_tag_check
	$(if $(word 2,$1),
		$(error Bad return value of '$(builtin_name)' push handler: '$1')
	)
	$(firstword $1)
endef

# Increments the number of arguments of the function call being handled now.
# Return:
#   Nothing.
define __def_outer_hook_arg
	$(call __def_debug,arg$(words $(__def_stack_top)))
	${eval \
		$(__def_stack_arg_mk)
	}
endef

# Removes an element from the top and restores the previous element.
# Return:
#   Nothing.
define __def_outer_hook_pop
	${eval \
		$(__def_stack_pop_mk)
	}
	$(call __def_debug,pop)
endef

# Handles a function expansion. Performs generic checks (arity, ...) and
# a special translation in case of user-defined builtin.
# Return:
#   A code which will substitute the original expansion.
define __def_outer_hook_func
	$(if $(value __def_debug),$(call __def_debug,func [$(builtin_reconstruct)]))
	$(foreach 0,$(builtin_name),
		$(if $(call var_defined,builtin_func-$0),
			# There is a special builtin function handler, invoke it.
			$(builtin_func-$0),

			# If it is a native function check its arity.
			$(foreach minimum_args,
				# For unknown function 'minimum_args' would be 0.
				$(or $(notdir $(filter $0/%,$(__builtin_native_functions))),0),

				$(if $(minimum_args:0=),
					$(call builtin_check_min_arity,$(minimum_args)),
					$(call builtin_error,
						Undefined function '$0'
					)
				)
			)
			# Finally leave the function call as is.
			$(builtin_reconstruct)
		)
	)
	$(__def_outer_hook_pop)
endef

# Issues a warning with the specified message including the expansion stack.
# Params:
#   1. Code fragment which caused the warning.
#   2. The message.
# Return:
#   The first argument.
define __def_outer_hook_warning
	$1
	$(warning $2$(if $1,: '$1'))
	$(builtin_print_stack)
endef

# Special builtin which echoes its arguments.
define builtin_func-__def_root__
	$(builtin_args)
endef

# List of GNU Make 3.81 native builtin functions with their arities.
__builtin_native_functions := \
	abspath/1    \
	addprefix/2  \
	addsuffix/2  \
	basename/1   \
	dir/1        \
	notdir/1     \
	subst/3      \
	suffix/1     \
	filter/2     \
	filter-out/2 \
	findstring/2 \
	firstword/1  \
	flavor/1     \
	join/2       \
	lastword/1   \
	patsubst/3   \
	realpath/1   \
	shell/1      \
	sort/1       \
	strip/1      \
	wildcard/1   \
	word/2       \
	wordlist/3   \
	words/1      \
	origin/1     \
	foreach/3    \
	call/1       \
	info/1       \
	error/1      \
	warning/1    \
	if/2         \
	or/1         \
	and/1        \
	value/1      \
	eval/1

#
# Here goes an API for defining own builtin functions.
#

# Reconstructs original builtin invocation code.
# Note:
#   This is a macro and you must not 'call' it.
builtin_reconstruct = \
	$$($(builtin_name) $(builtin_args))

# Gets the name of a function being handled.
# Example:
#   For $(foo bar,baz) it would return 'foo'.
# Note:
#   You may simply expand this macro without 'call'ing it.
#   In most cases you may also use $0 instead.
builtin_name = \
	$(firstword $(__def_stack_top))

# Gets a list of variable names of all arguments of a function being handled.
# This macro is useful for counting and iterating over the formal arguments.
# Example:
#   For $(foo bar,baz) it would return '1 2'.
# Note:
#   You may simply expand this macro without 'call'ing it.
builtin_args_list = \
	$(call nofirstword,$(__def_stack_top))

#
# Accessing the actual argument passed to the builtin.
#
# 'builtin_args' and its derivatives construct an actual arguments passing
# code in a 'call'-like syntax.
#
# Note:
#   You must not 'call' theese macros in order to preserve argument values of
#   the current function call.
#

# $(f foo,bar,baz) -> 'foo,bar,baz'.
builtin_args      = $(foreach args_filter,id         ,$(__builtin_args_expand))

# $(f foo,bar,baz) -> 'foo'.
# Note: You will probably consider using more conveniet '$1' instead.
builtin_firstarg  = $1

# $(f foo,bar,baz) -> 'baz'.
builtin_lastarg   = $(foreach args_filter,lastword   ,$(__builtin_args_expand))

# $(f foo,bar,baz) -> 'bar,baz'.
builtin_nofirstarg= $(foreach args_filter,nofirstword,$(__builtin_args_expand))

# $(f foo,bar,baz) -> 'foo,bar'.
builtin_nolastarg = $(foreach args_filter,nolastword ,$(__builtin_args_expand))

# Comma-separated list of expanded arguments. Intended for internal usage only.
# Context:
#   'args_filter': name of a filtering function to apply to the list of args.
# Example:
#   For $(func foo,bar,baz) it would return 'foo,bar,baz'.
# Note:
#   You must not 'call' this macro in order to preserve the current context.
define __builtin_args_expand
	# Can't use '__def_expand' because of the need to access local arguments.
	# But everything we have told about there concerns to the code below too.
	${eval \
		__def_tmp__ := \
			# For $(func foo,bar,baz) it would be '$(1),$(2),$(3)'
			# which in turn expands to the sought-for.
			$(subst $(\s),$(\comma),$(patsubst %,$$(%),
				$(call $(args_filter),$(builtin_args_list))
			))
	}
	$(__def_tmp__)
endef

#
# Arity checks: exact, min, max, range.
#

# Asserts that the actual number of arguments equals to the specified value
# Otherwise it fails with an error.
# Params:
#   1. Exact required function arity.
# Return:
#   Nothing.
builtin_check_arity = \
	$(call builtin_check_arity_range,$1,$1)

# Asserts there are at least n arguments passed to the builtin.
# Otherwise it fails with an error.
# Params:
#   1. Minimal required function arity.
# Return:
#   Nothing.
builtin_check_min_arity = \
	$(call builtin_check_arity_range,$1,)

# Asserts there are at most m arguments passed to the builtin.
# Otherwise it fails with an error.
# Params:
#   1. Maximum allowed function arity.
# Return:
#   Nothing.
builtin_check_max_arity = \
	$(call builtin_check_arity_range,,$1)

# Asserts that the actual number of arguments falls within the specified range.
# Otherwise it fails with an error.
# Params:
#   1. Minimal required function arity or empty if lower boundary is not
#      specified.
#   2. Maxumum allowed arity or empty if there is no restriction.
# Return:
#   Nothing.
define builtin_check_arity_range
	$(call __builtin_check_arity_range,
		$(wordlist \
			$(or $(1:0=),1),
			$(or $2,$(words $(builtin_args_list))),
			$(builtin_args_list)
		)
	)
endef

# Params:
#   1. List of args: min .. max
define __builtin_check_arity_range
	$(if $(filter-out $1,$(words $(builtin_args_list))),
		$(call builtin_error,
			Too $(if $1,many,few) arguments ($(words $(builtin_args_list))) \
			to function '$(builtin_name)'
		)
	)
endef

#
# Inspecting the expansion stack of your builtin.
#

# Gets names of functions in the expansion stack starting from direct caller
# and up to the root.
# Return:
#   Whitespace-separated list of function names.
# Example:
#   For the function 'baz' in $(foo $(bar $(baz ...))) the return would be
#   'bar foo'.
define builtin_callers
	$(filter-out $(\comma)%,$(subst $(\comma), $(\comma),
		$(filter-out __paren__% __def_root__%,$(__def_stack))
	))
endef

# A shorthand for $(firstword $(builtin_callers)).
# Return:
#   Name of the direct caller of the current function (if any).
define builtin_caller
	$(firstword $(builtin_callers))
endef

# A shorthand for $(word depth,$(builtin_callers)).
# Gets the name of a function which is upper in the expansion stack than the
# current one by the specified depth.
# Params:
#   1. Depth, where '1' means direct caller.
# Return:
#   Function name or empty if the argument is bigger than actual stack depth.
# Example:
#   In case of handling the innermost function of $(foo $(bar $(baz ...))),
#   namely 'baz', its direct caller is 'bar' and a caller at depth 2 is 'foo'.
define builtin_caller_at
	$(word $1,$(builtin_callers))
endef

# Retrieves a tag if it has been defined.
#
# Params:
#   1. Function name.
# Return:
#   The tag of the nearest expansion of the specified builtin (if any).
# Example:
#   In case of handling 'baz' builtin in expression $(foo $(bar $(baz ...))),
#   and assuming that 'builtin_tag-bar' returned 'moo', the result of calling
#   '$(call builtin_tag,bar)' will be 'moo'.
define builtin_tag
	$(value __def_outer_tag_$1)
endef

#
# Helper functions for error/warning reporting.
#

# Outputs the expansion stack.
# Return:
#   Nothing.
define builtin_print_stack
	$(warning Expansion stack:)
	$(warning $(\t)function '$(firstword $(__def_stack_top))')
	$(strip $(foreach e,$(__def_stack),
		$(warning \
			$(\t)arg $(words $(call nofirstword,$(subst $(\comma), ,$e))) \
				of '$(firstword $(subst $(\comma), ,$e))'
		)
	))
endef

# Produces a warning with the specified message and contents of the expansion
# stack.
# Params:
#   1. Warning message.
# Return:
#   Nothing.
define builtin_warning
	$(warning $1)
	$(builtin_print_stack)
endef

# Fatal version of 'builtin_warning'.
# Params:
#   1. Error message.
# Return:
#   No return.
define builtin_error
	$(builtin_warning)
	$(error Error in definition of '$(__def_var)' function)
endef

#
# Misc.
#

# Helper for auxiliary function allocation.
# Useful if your builtin needs to define auxiliary function/variable.
# Return:
#   A unique name in a private namespace.
define builtin_aux_alloc
	${eval \
		__builtin_aux_cnt += x
	}
	$(builtin_aux_last)
endef
__builtin_aux_cnt :=# Initially empty.

# Allocates a new auxiliary function and assigns a value to it.
# Params:
#   1. A single-line value to assign.
# Return:
#   The name of the newly defined function.
define builtin_aux_def
	$(foreach aux,$(builtin_aux_alloc),
		$(call var_assign_recursive_sl,$(aux),$1)
		$(aux)# Return.
	)
endef

# Gets the last allocated name.
# Return:
#   Result of the last call to 'builtin_aux_alloc' or to 'builtin_aux_def'.
define builtin_aux_last
	__def_aux$(words $(__builtin_aux_cnt))
endef

# Turn off auto-def for functions generated by builtin handlers.
__def_ignore += __def_aux%

# Ufff, builtins definition framework is mostly up.
# Flush functions we have just defined and activate '__def_builtin_real'.
$(def_all)

__def_builtin = \
	$(call __def_builtin_real,$1)

#
# Define some simple builtins that will help us with defining the rest ones.
#

#
# Extension: 'assert' builtin function.
#
# Runtime assertions.
#
# '$(assert condition[,message...])'
#
define builtin_func-assert
	$$(if $1,,
		$$(call __assert_handle_failure,$(__def_var),$(subst $$,$$$$,$1)
			$(if $(filter 2,$(builtin_args_list)),
				$(\comma)$(subst $(\comma),$$(\comma),$(builtin_nofirstarg))
			)
		)
	)
endef

# Params:
#   1. Function name.
#   2. Expression.
#   3. Optional message.
# No return.
define __assert_handle_failure
	$(call $(if $(__def_stack_top),builtin_error,error),\
		ASSERTION FAILED in function '$1': '$2'$(if $(value 3),: $3)
	)
endef

#
# Extension: 'lambda' builtin function.
#
# Def-time anonymous function definition.
#
# '$(lambda body)'
#
define builtin_func-lambda
	$(call builtin_aux_def,$(builtin_args))
endef

# Stub for case of $(lambda) or $(call lambda,...).
lambda = \
	$(warning lambda: illegal invocation)

#
# Extension: 'with' builtin function.
#
# Def-time function definition with args applied at the runtime.
#
# '$(with args...,body)'
#
define builtin_func-with
	$$(call $(call builtin_aux_def,$(builtin_lastarg))
		$(if $(call nolastword,$(builtin_args_list)),
			$(\comma)$(builtin_nolastarg)
		)
	)
endef

# Stub for case of plain $(with) or $(call with,...).
with = \
	$(warning with: illegal invocation)

#
# Extension: 'expand' builtin function.
#
# Expands the argument inside the context of the caller function.
#
# '$(expand code...)'
#
define builtin_func-expand
	$${eval \
		__def_tmp__ := \
			$$$$(\0)# Preserve leading whitespace.
			$$(subst $$(\n),$$$$(\n),# Escape newlines.
				$$(subst $$(\h),$$$$(\h),# Do not treat hashes as comments.
					$(subst $(\h),$$$$(\h),$(builtin_args))
				)
			)
	}
	$$(__def_tmp__)
endef

# Flush: assert, lambda, with and expand.
$(def_all)

expand = $(expand $1)
$(call def,expand)

#
# Extension: 'fx' builtin function.
#
# Runtime partial function application.
#
# '$(fx func,args...)'
#
define builtin_func-fx
	$(with \
		$(expand \
			$(subst $(\comma)$(\s),$(\comma),
				$(foreach arg,$(builtin_args_list),
					$(if $(findstring $$,$(subst $$$$,,$($(arg)))),
						$(call list_fold,
							$(lambda $$$$(subst $$$$($2),$$$$$$$${$2},$1)),
							$$$$(subst $$$$$$$$,$$$$$$$$$$$$$$$$,$$($(arg))),
							\comma [ ]
						),
						$(subst $$,$$$$$$$$,$($(arg)))
					)
					$(\comma)
				)
			)
		),
		$(if $(findstring $$,$(subst $$$$$$$$,,$1)),
			$$(foreach fn,__fx$$(words $$(__builtin_func-fx_cnt)),
				$$(eval \
					__builtin_func-fx_cnt += x$$(\n)
					define $$(fn)$$(\n)$$$$(call $1$$$$1)$$(\n)endef
				)
				$$(fn)
			),
			$(call builtin_aux_def,$$(call $(subst $$$$,$$,$1)$$1))
		)
	)
endef

__builtin_func-fx_cnt :=# Initially empty.

#
# Builtin to user-defined function call converters.
#

# Converts the builtin into a generic 'call' of a user-defined function.
# For example, $(foo bar,baz) becomes $(call foo,bar,baz).
#
# Return:
#   The code transormed to a function call.
#
# Note:
#   If you want to use another name for target of 'call' being constructed
#   (e.g. 'my_foo' instead of 'foo') override 'builtin_name' variable using
#   'foreach': $(foreach builtin_name,my_foo,$(builtin_to_function_call))
#
# Note:
#   This is a macro and you must not 'call' it.
define builtin_to_function_call
	$(if $(filter undefined,$(flavor $(builtin_name))),
		$(call builtin_warning,
			Converting builtin into a call to possibly undefined function \
			'$(builtin_name)'
		)
	)
	$$(call \
		$(builtin_name),
		$(builtin_args)
	)
endef

# Tries to substitute the builtin by an inlined call to a user-defined
# function.
#
# In case when inlining of some arguments might lead to possible
# side effects (namely when an argument contains a call to any function or
# references a variable which is not proven to be simply expanded, and that
# argument is used in the target function more than once or is not used at all)
# then this routine falls back to using regular function call.
#
# It is important to notice that only a static textual substitution of formal
# argument references is performed.
#
# This routine does NOT recognize:
#   - computed variable references:
#       foo = $(foreach a,1 2 3,$($a)) # error
#
#   - substitution references within arguments:
#       foo = $(1:bar=baz) # error
#
#   - getting the value of an argument using 'value' and 'call' functions:
#       foo = $(call 1) $(value 2) # error
#
#   - referencing an argument within a macro expansion of some other
#     recursive variable:
#       foo = $(bar) # error
#       bar = $1
#
# You must NOT try to inline such functions: the call would be inlined
# improperly with no errors/warnings reported.
#
# Return:
#   The code transormed to a function call (may be inlined).
#
# Example:
#   Builtin function 'eq' is marked to be inlineable and the corresponding
#   function is defined as follows:
#     eq = $(findstring $1,$(findstring $2,$1))
#
#   As you can see, the first argument is used twice. Thus in some cases the
#   function call can't be inlined.
#
#     $(eq foo,bar)     # OK, no expansion references at all.
#                       # $(findstring foo,$(findstring bar,foo))
#
#     $(eq foo,$(bar))  # OK, $(bar) is used only once inside 'eq'.
#                       # $(findstring foo,$(findstring $(bar),foo))
#
#     $(eq $3,bar)      # OK, $3 is automatic variable and it is always simple.
#                       # $(findstring $3,$(findstring bar,$3))
#
#     $(eq $(foo),bar)  # Failure, $(foo) would be expanded twice.
#                       # Using regular call:
#                       # $(call eq,$(foo),bar)
#
# Note:
#   Inlining may break the order of arguments expansion. Preserving the right
#   order is a responsibility of the target function being inlined.
#
# Note:
#   See notes of 'builtin_to_function_call' if you want to inline a function
#   named another then the builtin itself.
#
# Note:
#   This is a macro and you must not 'call' it.
define builtin_to_function_inline
	$(or \
		$(if $(call not,$(call var_recursive,$(builtin_name))),
			$(if $(call var_simple,$(builtin_name)),
				$(call builtin_warning,
					Can not inline non-recursive variable '$(builtin_name)'
				),
				$(warning \
					Can not inline undefined function '$(builtin_name)'
				)
			),
			$(__builtin_to_function_inline)
		),
		$(builtin_to_function_call)
	)
endef

# Actual inlining is performed here.
# Return:
#   The transormed code if inlining succeeds, empty otherwise.
# Note:
#   This is a macro and you must not 'call' it.
define __builtin_to_function_inline
	$(with \
		# We'll populate the following variable with names of the arguments
		# being inlined.
		${eval \
			__builtin_to_function_inline_expanded_args :=# Reset.
		}
		# Inlining is actually performed by the expansion engine.
		# We just escape everything except recognized argument references and
		# expand it in the current context.
		$(expand \
			$(call list_fold,
				$(lambda \
					$(subst $$$$($2),$$(foreach arg,$2,$$($3)),
						$(if $(filter 1 2 3 4 5 6 7 8 9,$2),
							$(subst $$$$$2,$$$$($2),$1),
							$1
						)
					)
				),
				$(subst $$$$$$$$,$$($$)$$($$),$(subst $$,$$$$,
					# Any usage of arg 0 is replaced by possibly overridden
					# value of 'builtin_name',
					# not the real name of the builtin stored in $0.
					$(subst $$(0),$(builtin_name),$(subst $$0,$(builtin_name),
						$(value $(builtin_name))
					))
				)),
				$(builtin_args_list),
				$(lambda \
					${eval \
						__builtin_to_function_inline_expanded_args += $(arg)
					}
					$($(arg))
				)
			)
		),
		# After the inlining is done we have to check the list of the arguments
		# that have been inlined.
		$(strip $(foreach arg,$(builtin_args_list),
			$(and \
				# Check the presence of each argument in the list of
				# actually inlined arguments that we have collected during
				# the expansion. The argument should be listed exactly once.
				$(call not,$(call singleword,$(filter $(arg),
						$(__builtin_to_function_inline_expanded_args)))),

				# Well, the argument has been inlined more than once or has not
				# been used at all. Check if the value of the argument contains
				# any expansion that could lead to possible side effects.
				# The check itself is rather simple: we just search the value
				# of the argument for any unescaped dollars that do not refer
				# to the first ten function call arguments ($0 .. $9). The
				# latters are guaranteed to be simply expanded variables with
				# no possible side effects or any performance overhead.
				$(findstring $$,
					$(call list_fold,
						$(lambda $(subst $$$2,,$(subst $$($2),,$1))),
						$(subst $$$$,,$($(arg))),
						0 1 2 3 4 5 6 7 8 9
					)
				),

				# Issue a debug message with the reason of ambiguity
				# and emit the name of the bad argument.
				$(if $(value __def_debug),
					$(call __def_debug,
						Value of the argument $(arg) ('$($(arg))') is \
						$(if $(filter $(arg),
								$(__builtin_to_function_inline_expanded_args)),
							used more than once,
							not used
						) inside the function being inlined
					)
				)
				$(arg)
			)
		)),

		$(if $2,
			$(if $(value __def_debug),
				$(call __def_debug,
					Inlining of function '$(builtin_name)' failed due to \
					ambiguous usage of certain arguments
				)
			),
			$1
		)
	)
endef

# Finally, flush the rest and say Goodbye!
$(def_all)

ifeq (0,1)
$(foreach __def_var, \
	$(filter-out \
		__def_debug \
		__def_stack \
		__def_stack_top \
		__%__ \
		  %_mk, \
		$(filter \
			  def% \
			__def%, \
			$(.VARIABLES) \
		) \
	), \
	$(call def,$(__def_var)) \
)
endif

endif # __core_def_mk
