#
# Copyright 2008-2010, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2008-2010, Lanit-Tercom Inc. All rights reserved.
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
# Integer math.
#
# Author: Eldar Abusalimov
#

ifndef __util_math2_mk
__util_math2_mk := 1

include core/common.mk
include util/list.mk

int_decode = \
  $(subst $(\space),,$(call list_reverse,$1))

int_encode = \
  $(call list_reverse,$(call list_fold,__int_encode_fold,$1,$(__int_dec_nrs)))
__int_encode_fold = $(subst $2, $2 ,$1)

__int_dec_nrs := 0 1 2 3 4 5 6 7 8 9

__int_add_0 := 0:1 0:2 0:3 0:4 0:5 0:6 0:7 0:8 0:9
__int_add_1 := 0:2 0:3 0:4 0:5 0:6 0:7 0:8 0:9 1:0
__int_add_2 := 0:3 0:4 0:5 0:6 0:7 0:8 0:9 1:0 1:1
__int_add_3 := 0:4 0:5 0:6 0:7 0:8 0:9 1:0 1:1 1:2
__int_add_4 := 0:5 0:6 0:7 0:8 0:9 1:0 1:1 1:2 1:3
__int_add_5 := 0:6 0:7 0:8 0:9 1:0 1:1 1:2 1:3 1:4
__int_add_6 := 0:7 0:8 0:9 1:0 1:1 1:2 1:3 1:4 1:5
__int_add_7 := 0:8 0:9 1:0 1:1 1:2 1:3 1:4 1:5 1:6
__int_add_8 := 0:9 1:0 1:1 1:2 1:3 1:4 1:5 1:6 1:7
__int_add_9 := 1:0 1:1 1:2 1:3 1:4 1:5 1:6 1:7 1:8

__int_mul_0 := 0:0 0:0 0:0 0:0 0:0 0:0 0:0 0:0 0:0
__int_mul_1 := 0:1 0:2 0:3 0:4 0:5 0:6 0:7 0:8 0:9
__int_mul_2 := 0:2 0:4 0:6 0:8 1:0 1:2 1:4 1:6 1:8
__int_mul_3 := 0:3 0:6 0:9 1:2 1:5 1:8 2:1 2:4 2:7
__int_mul_4 := 0:4 0:8 1:2 1:6 2:0 2:4 2:8 3:2 3:6
__int_mul_5 := 0:5 1:0 1:5 2:0 2:5 3:0 3:5 4:0 4:5
__int_mul_6 := 0:6 1:2 1:8 2:4 3:0 3:6 4:2 4:8 5:4
__int_mul_7 := 0:7 1:4 2:1 2:8 3:5 4:2 4:9 5:6 6:3
__int_mul_8 := 0:8 1:6 2:4 3:2 4:0 4:8 5:6 6:4 7:2
__int_mul_9 := 0:9 1:8 2:7 3:6 4:5 5:4 6:3 7:2 8:1

__int_dec_nrs_lt = \
  $(wordlist 1,$1,0 1 2 3 4 5 6 7 8 9)


endif # __util_math2_mk
