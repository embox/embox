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
#    - Function definition with inline comments
#    - Defining multiline verbose functions
#    - Using tabs for function code indentation
#  - Semantic:
#    - 'lambda' and 'with' for defining anonymous inner functions
#
#   Date: Jun 28, 2011
# Author: Eldar Abusalimov
#

ifndef __core_define_mk
__core_define_mk := 1

include core/common.mk
include core/string.mk

include util/list.mk
include util/var/assign.mk

$(or define) = $(strip \
  $(foreach __define_var,$1, \
    $(call var_assign_recursive_sl,$(__define_var),$ \
      $(call __define,$(value $(__define_var))))) \
)

# 1. Text
__define = \
    $(call __define_builtin,$ \
      $(call __define_brace,$ \
        $(call __define_strip,$ \
          $(subst $$,$$$$,$1)))

# 1. Text
__define_strip = \
  $(call __define_strip_postprocess,$ \
    $(call list_scan,__define_strip_fold,_$$n,$ \
      $(call __define_strip_preprocess,$ \
       $1)))

# 1. Text
__define_strip_preprocess = \
  $(subst \ _$$h ,$(\h),$ \
   $(subst  $(\h), _$$h ,$ \
    $(subst  $(\n), _$$n ,$ \
     $(subst  $(\t), _$$t ,$ \
      $(subst  $(\s), _$$s ,$ \
       $1)))))

# 1. Previous token or empty inside a comment
# 2. Current token
# Return: token to append to the resulting text
__define_strip_fold = \
  $(and $(subst _$$h,,$2), \
        $(if $(findstring _$$n,$1),$ \
             $(subst _$$s,_$$n,$(subst _$$t,_$$n,$2)),$ \
             $(or $(findstring _$$n,$2), \
                  $(if $1,$2))) \
   )

# 1. Text
__define_strip_postprocess = \
  $(subst  _$$s,$(\s),$ \
   $(subst  _$$t,$(\t),$ \
    $(subst  _$$n,,$      \
     $(subst $(\s),,$      \
      $1))))

#
# Starting at this point the rest functions will be written using new syntax
# that we have just defined.
# But since '__define_brace' and '__define_builtin' functions have not been
# implemented yet, we need to define stubs conforming to their interfaces.
#

# See '__define_brace_real'.
__define_brace = \
  $(subst $$$$,$$,$1)

# See: '__define_builtin_real'.
__define_builtin = $1

# Now we can use $(call define,...)

# Expands value of the argument treating it as a code.
# Params:
#   1. Make code to be expanded.
# Return:
#   Expansion result.
# Note:
#   As a side effect all escaped dollars become unescaped.
define __define_expand
	${eval \
		__define_expand_tmp__ := $1
	}
	$(__define_expand_tmp__)
endef
$(call define,__define_expand)

# Substitutes all unescaped occurrences of ${...} expansion to $(...).
# This does not affect pure {...} groups or escaped $${...}.
# Params:
#   1. Dollar-escaped value.
# Return:
#   The unescaped result of brace substitution.
define __define_brace_real
	# Restore doubled dollars back.
	$(subst $$$$ ,$$$$,
		# Expand hooks.
		$(call __define_expand,
			# Replace true ${...} expansion occurrences to a hook call:
			#   ${call __define_brace_hook,...}
			# which will echo its argument surrounded by $(...)
			$(subst $$$${,$${call __define_brace_hook$(\comma),
				# Replace commas with references to \comma.
				# This is needed to get properly handling of cases like
				#   $(call foo,${xxx bar,baz})
				# that would otherwise result in an error:
				#   unterminated call to function `xxx': missing `}'
				$(subst $(\comma),$$(\comma),
					# First of all, isolate doubled dollars (in terms of the
					# original value) from the succeeding text.
					#   $${...}  -> $$ {...}
					#   $$${...} -> $$ ${...}
					$(subst $$$$$$$$,$$$$$$$$ ,
						$1
					)
				)
			)
		)
	)
endef
$(call define,__define_brace_real)

# Surrounds the argument with '$(' and ')'.
__define_brace_hook = \
  $$($1)

# Brace-to-paren substitution logic is now ready.
__define_brace = \
  $(__define_brace_real)

# We will populate this list of extensions on by one.
__define_builtin_extensions :=# Initially empty.

# The real work is done here.
# 1. Text
define __define_builtin_real
	$(subst $(\comma)$$,$(\comma),
		# This will force each builtin keyword occurrence to reflect
		# in __define_builtin_func_xxx invocation.
		$(call __define_expand,
			$(subst \
				$(\comma),
				$(\comma)$$(call __define_expand_arg_hook),
				$(call list_fold,__define_builtin_fold,
					$(subst $$,$$$$,$1),
					$(__define_builtin_extensions))
			)
		)
	)
endef
$(call define,__define_builtin_real)

# 1. Text
# 2. Built-in extension function name ('lambda', 'with', ...)
define __define_builtin_fold
	$(subst \
		$$$$$(\paren_open)$2 ,
		$$$(\paren_open)call __define_builtin_func_$2$(\comma),
		$1)
endef
$(call define,__define_builtin_fold)

#define __define_builtin_fold
#	$(subst \
#		$$$$$(\paren_open)$2 ,
#		$$$(\paren_open)call
#			$$(call __define_expand_func_hook,$2)
#			__define_builtin_hook$(\comma),
#		$1)
#endef
#$(call define,__define_builtin_fold)

# Now builtins definition core framework is up. Enable it here.
__define_builtin = \
  $(__define_builtin_real)

#
# Auxiliary functions: arguments check and function allocation.
#

# Calling '__define_builtin_args_eval' results in setting this variable:
__define_builtin_args :=# Sequence '1 .. N', e.g. '1 2 3'.

# Calculate the list of arguments available in the context of expansion
# and store the result into '__define_builtin_args' variable.
define __define_builtin_args_eval
	${eval \
		__define_builtin_args :=# Reset the value.
	}
	# Can't use 'call' here because it would destroy
	# all currently eligible arguments.
	$(foreach 0,__define_builtin_args_loop,
		$($0)
	)
endef
$(call define,__define_builtin_args_eval)

# Loops over names of non-empty arguments available it the current context and
# populates '__define_builtin_args'.
define __define_builtin_args_loop
	# Increment 'arg_nr' to get name of the next argument being checked.
	$(foreach arg_nr,$(words $(__define_builtin_args) x),
		$(if $(value $(arg_nr)),
			# Found non-empty argument.
			${eval \

				# Append the argument name to the result.
				__define_builtin_args += $(arg_nr)$(\n)

				# As mentioned above we can't use 'call' function to dive into
				# recursive subcall of this function. Instead we just evaluate
				# its value with updated '__define_builtin_args'. This hack
				# prevents 'Recursive variable references itself' error.

				# Assuming that the expansion has no non-whitespace.
				$(value $0)
			}
		)
	)
endef
$(call define,__define_builtin_args_loop)

# Useful if your builtin needs to define auxiliary function/variable.
define __define_builtin_alloc
	__builtin$(words $(__define_builtin_cnt))$(__define_var)
	${eval \
		__define_builtin_cnt += x
	}
endef
$(call define,__define_builtin_alloc)
__define_builtin_cnt :=# Initially empty.

#
# Extension: 'lambda' builtin function.
#
# '$(lambda $(foo)...)' -> '__builtin000'
# where __builtin000 = $(foo)...
# ... Arguments to 'lambda' with '$ ' prepended: '$ $(foo)...'.
define __define_builtin_func_lambda
	$(foreach func_name,$(__define_builtin_alloc),
		# Define external function
		${eval \
			$(func_name) = $(call nofirstword,$1)
		}
		# The value being returned.
		# $(or ) is used to handle escaped lambdas properly. For example:
		# $$(lambda ...) -> $$(or )__builtin000
		$$(or )$(func_name)
	)
endef
$(call define,__define_builtin_func_lambda)
__define_builtin_extensions += lambda

ifeq (0,1)
#
# Extension: 'with' builtin function.
#
# '$(with 1,2,foo$1 bar$2)' -> '$(call __builtin000,1,2)'
# where __builtin000 = foo$1 bar$2
# ... Arguments to 'with' each with leading '$ ': '$ 1,$ 2,$ foo$1 bar$2'.
__define_builtin_func_with = \
  $(foreach __define_var,$(__define_builtin_alloc),$ \
    $(__define_builtin_args_eval)${eval $(__define_var) = \
        $(call nofirstword,$($(words $(__define_builtin_args))))}$ \
    $(__define_builtin_func_with_ret))

__define_builtin_func_with_ret = \
  $$(call $(__define_var)$(subst $(\s)$(\comma),$(\comma), $ \
      $(foreach arg,$(call nolastword,$(__define_builtin_args)),$ \
        $(\comma)$($(arg)))))

__define_builtin_func_with_args = \
  $$(call $(__define_var)$(subst $(\s)$(\comma),$(\comma), $ \
      $(foreach arg,$(call nolastword,$(__define_builtin_args)),$ \
        $(\comma)$($(arg))))))# <- The value being returned.


#
# Extension: 'match' builtin function.
#
#  $(match $(foo),
#      $(call pattern1,$~),
#          expr1 $~,
#      pattern2,
#          expr2
#   )
# ->
__define_builtin_func_with = \
  $(foreach __define_var,$(__define_builtin_alloc),$ \
    ${eval $(__define_var) :=}$ \
    $$(call $(__define_var)$(subst $(\s)$(\comma),$(\comma), $ \
      $(foreach arg,$(call nolastword,$(__define_builtin_args)),$ \
        $(\comma)$($(arg))))))# <- The value being returned.

define __define_builtin_func_match_case_mk
  __match_tmp := $1
  ifdef __match_tmp
    override ~ := $$(__match_tmp)
    override ~ := $2
    __match_tmp := x
  endif
endef
  ${eval __builtin000 := $$(value ~)}
  ${eval ~ := $$(foo)}
  $(if $(or \
       ${eval \
           __match_tmp := $$(call pattern1,$$~)$ \
           }$ \
           $(if $(__match_tmp),x${eval \
               override ~ := $$(__match_tmp)
               override ~ := expr1 $$~$ \
           }),
       ${eval ~ := pattern2}$(if $~,x${eval ~ := expr2})
  ),$~)
  ${eval ~ := $$(__builtin000)}
  $(if $(or \
       ${eval \
           __match_tmp := $$(call pattern1,$$~)
           ifdef __match_tmp
               override ~ := $$(__match_tmp)
               override ~ := expr1 $$~
               __match_tmp := x
           endif
        }$(__match_tmp)
       ${eval ~ := pattern2}$(if $~,x${eval ~ := expr2})
  ),$(__match_tmp))

  ${eval __builtin000 := $$(value ~)}
  ${eval __builtin001 := $$(foo)}
  $(or
       ${eval ~ := $$(__builtin001)}
       ${eval __builtin001 := $$(call pattern1,$$~)}$ \
           $(if $(__builtin001),x${eval \
               override ~ := $$(__builtin001)
               __builtin001 := expr1 $$~$ \
           }),
       ${eval ~ := pattern2}$(if $~,x${eval ~ := expr2})
  )$~
  ${eval ~ := $$(__builtin000)}

  __builtin000 := $(value ~)

  override ~ := $(foo)
ifdef __builtin001${eval __builtin001 := $$(call pattern1,$$~)}
  override ~ := $(__builtin001)
  __builtin001 := expr1 $~
else ifdef __builtin001${eval __builtin001 := pattern2}
  override ~ := $(__builtin001)
  __builtin001 := expr2
else
  __builtin001 :=
endif

  override ~ := $(__builtin000)

#
#
# where __builtin000 = foo$1 bar$2
# ... Arguments to 'with' each with leading '$ ': '$ 1,$ 2,$ foo$1 bar$2'.
__define_builtin_func_with = \
  $(foreach __define_var,$(__define_builtin_alloc),$ \
    ${eval $(__define_var) = \
        $(call nofirstword,$($(words $(__define_builtin_args))))}$ \
    $$(call $(__define_var)$(subst $(\s)$(\comma),$(\comma), $ \
      $(foreach arg,$(call nolastword,$(__define_builtin_args)),$ \
        $(\comma)$($(arg))))))# <- The value being returned.

endif

endif # __core_define_mk
