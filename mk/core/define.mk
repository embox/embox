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
#    - Using tabs for function indentation
#    - Using tabs for function indentation
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

$(or define) = \
  $(call var_assign_recursive_sl,$1,$ \
    $(call __define,$(value $(or $(value 2),$1))))

# 1. Text
__define = \
    $(call __define_lambda,$ \
      $(call __define_strip,$ \
        $(subst $$,$$$$,$1)))

# 1. Text
__define_lambda = \
  $(call __define_lambda_expand, \
    $(call list_fold,__define_lambda_fold,$1,$(\paren_open) $(\brace_open)))

# 1. Text
# 2. Open bracket
__define_lambda_fold = \
  $(subst $$$$$2lambda ,$$$2call __define_lambda_hook$(\comma),$1)

# Forces each 'lambda' and 'with' occurrence to reflect
# in __define_lambda_hook invocation.
# 1. Text
__define_lambda_expand = \
  ${eval __define_lambda_tmp__ := $1}$(__define_lambda_tmp__)

__define_lambda_hook = $(info $0: [$1])info

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

endif # __core_define_mk

