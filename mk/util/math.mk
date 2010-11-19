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

# Most parts of this file are derived from GMSL.

#
# GNU Make Standard Library (GMSL)
#
# A library of functions to be used with GNU Make's $(call) that
# provides functionality not available in standard GNU Make.
#
# Copyright (c) 2005-2010 John Graham-Cumming
#
# This file is part of GMSL
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# Neither the name of the John Graham-Cumming nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#
# Integer math.
#
# Author: Eldar Abusalimov
#

# Integers a represented by lists with the equivalent number of x's.
# For example the number 4 is x x x x.  The maximum integer that the
# library can handle as _input_ is __gmsl_input_int which is defined
# here as 65536

__gmsl_sixteen := x x x x x x x x x x x x x x x x
__gmsl_input_int := $(foreach a,$(__gmsl_sixteen),         \
                        $(foreach b,$(__gmsl_sixteen),     \
                            $(foreach c,$(__gmsl_sixteen), \
                                $(__gmsl_sixteen)))))

#
# Function:  int_decode
# Arguments: 1: A number of x's representation
# Returns:   Returns the integer for human consumption that is represented
#            by the string of x's
#
int_decode = $(words $1)

#
# Function:  int_encode
# Arguments: 1: A number in human-readable integer form
# Returns:   Returns the integer encoded as a string of x's
#
int_encode = $(wordlist 1,$1,$(__gmsl_input_int))

# The arithmetic library functions come in two forms: one form of each
# function takes integers as arguments and the other form takes the
# encoded form (x's created by a call to int_encode).  For example,
# there are two plus functions:
#
# plus        Called with integer arguments and returns an integer
# int_plus    Called with encoded arguments and returns an encoded result
#
# plus will be slower than int_plus because its arguments and result
# have to be translated between the x's format and integers.  If doing
# a complex calculation use the int_* forms with a single encoding of
# inputs and single decoding of the output.  For simple calculations
# the direct forms can be used.

# Helper function used to wrap an int_* function into a function that
# takes a pair of integers, perhaps a function and returns an integer
# result
__gmsl_int_wrap = $(call int_decode,$(call $1,$(call int_encode,$2),$(call int_encode,$3)))
__gmsl_int_wrap1 = $(call int_decode,$(call $1,$(call int_encode,$2)))
__gmsl_int_wrap2 = $(call $1,$(call int_encode,$2),$(call int_encode,$3))

#
# Function:  int_plus
# Arguments: 1: A number in x's representation
#            2: Another number in x's represntation
# Returns:   Returns the sum of the two numbers in x's representation
#
int_plus = $(strip $1 $2)

#
# Function:  plus (wrapped version of int_plus)
# Arguments: 1: An integer
#            2: Another integer
# Returns:   Returns the sum of the two integers
#
plus = $(call __gmsl_int_wrap,int_plus,$1,$2)

#
# Function:  int_subtract
# Arguments: 1: A number in x's representation
#            2: Another number in x's represntation
# Returns:   Returns the difference of the two numbers in x's representation,
#            or outputs an error on a numeric underflow
#
int_subtract = $(strip $(if $(call int_gte,$1,$2), \
                $(filter-out xx,$(join $1,$2)),                 \
                $(call __gmsl_warning,Subtraction underflow)))

#
# Function:  subtract (wrapped version of int_subtract)
# Arguments: 1: An integer
#            2: Another integer
# Returns:   Returns the difference of the two integers,
#            or outputs an error on a numeric underflow
#
subtract = $(call __gmsl_int_wrap,int_subtract,$1,$2)

#
# Function:  int_multiply
# Arguments: 1: A number in x's representation
#            2: Another number in x's represntation
# Returns:   Returns the product of the two numbers in x's representation
#
int_multiply = $(strip $(foreach a,$1,$2))

#
# Function:  multiply (wrapped version of int_multiply)
# Arguments: 1: An integer
#            2: Another integer
# Returns:   Returns the product of the two integers
#
multiply = $(call __gmsl_int_wrap,int_multiply,$1,$2)

#
# Function:  int_divide
# Arguments: 1: A number in x's representation
#            2: Another number in x's represntation
# Returns:   Returns the result of integer division of argument 1 divided
#            by argument 2 in x's representation
#
int_divide = $(strip $(if $2,                                 \
                 $(if $(call int_gte,$1,$2),                               \
                     x $(call int_divide,$(call int_subtract,$1,$2),$2),), \
                 $(call __gmsl_error,Division by zero)))

#
# Function:  divide (wrapped version of int_divide)
# Arguments: 1: An integer
#            2: Another integer
# Returns:   Returns the integer division of the first argument by the second
#
divide = $(call __gmsl_int_wrap,int_divide,$1,$2)

#
# Function:  int_max, int_min
# Arguments: 1: A number in x's representation
#            2: Another number in x's represntation
# Returns:   Returns the maximum or minimum of its arguments in x's
#            representation
#
int_max = $(subst xx,x,$(join $1,$2))
int_min = $(subst xx,x,$(filter xx,$(join $1,$2)))

#
# Function:  max, min
# Arguments: 1: An integer
#            2: Another integer
# Returns:   Returns the maximum or minimum of its integer arguments
#
max = $(call __gmsl_int_wrap,int_max,$1,$2)
min = $(call __gmsl_int_wrap,int_min,$1,$2)

#
# Function: int_gt, int_gte, int_lt, int_lte, int_eq, int_ne
# Arguments: Two x's representation numbers to be compared
# Returns:   $(true) or $(false)
#
# int_gt    First argument greater than second argument
# int_gte   First argument greater than or equal to second argument
# int_lt    First argument less than second argument
# int_lte   First argument less than or equal to second argument
# int_eq    First argument is numerically equal to the second argument
# int_ne    First argument is not numerically equal to the second argument
#
int_gt  = $(call __gmsl_make_bool \
  ,$(filter-out $(words $1),$(words $(filter xx,$(join $1,$2)))))
int_gte = $(call not,$(call int_gt,$2,$1))
int_lt  = $(call int_gt,$2,$1)
int_lte = $(call not,$(call int_gt,$1,$2))
int_eq  = $(call __gmsl_make_bool,$(filter $(words $1),$(words $2)))
int_ne  = $(call not,$(call int_eq,$1,$2))

#
# Function: gt, gte, lt, lte, eq, ne
# Arguments: Two integers to be compared
# Returns:   $(true) or $(false)
#
# gt    First argument greater than second argument
# gte   First argument greater than or equal to second argument
# lt    First argument less than second argument
# lte   First argument less than or equal to second argument
# eq    First argument is numerically equal to the second argument
# ne    First argument is not numerically equal to the second argument
#
gt  = $(call __gmsl_int_wrap2,int_gt,$1,$2)
gte = $(call __gmsl_int_wrap2,int_gte,$1,$2)
lt  = $(call __gmsl_int_wrap2,int_lt,$1,$2)
lte = $(call __gmsl_int_wrap2,int_lte,$1,$2)
eq  = $(call __gmsl_int_wrap2,int_eq,$1,$2)
ne  = $(call __gmsl_int_wrap2,int_ne,$1,$2)

# increment adds 1 to its argument, decrement subtracts 1.  Note that
# decrement does not range check and hence will not underflow, but
# will incorrectly say that 0 - 1 = 0

#
# Function:  int_inc
# Arguments: 1: A number in x's representation
# Returns:   The number incremented by 1 in x's representation
#
int_inc = $(strip $1 x)

#
# Function:  inc
# Arguments: 1: An integer
# Returns:   The argument incremented by 1
#
inc = $(call __gmsl_int_wrap1,int_inc,$1)

#
# Function:  int_dec
# Arguments: 1: A number in x's representation
# Returns:   The number decremented by 1 in x's representation
#
int_dec = $(strip $(if $(call sne,0,$(words $1)), \
              $(wordlist 2,$(words $1),$1),                    \
              $(call __gmsl_warning,Decrement underflow)))

#
# Function:  dec
# Arguments: 1: An integer
# Returns:   The argument decremented by 1
#
dec = $(call __gmsl_int_wrap1,int_dec,$1)

#
# Function: seq
#
# Generates a sequence with the specified boundaries.
#
# Arguments:
#  1. A boundary (integer)
#  2. Another boundary (integer)
# Returns: generated sequence from the first boundary to (or downto) the second
#
seq = $(call int_seq,$(call int_encode,$1),$(call int_encode,$2))

#
# Function: int_seq
# See: seq
#
# Arguments:
#  1. A boundary (in x's representation)
#  2. Another boundary (in x's representation)
# Returns: generated sequence from the first boundary to (or downto) the second
#
int_seq = $(strip $(if $(call int_lt,$1,$2), \
  $(call __int_seq_inc,$1,$2), \
  $(call __int_seq_dec,$2,$1) \
))
__int_seq = $(if $(call int_lt,$1,$2),$(call $0,$(call int_inc,$1),$2))
# inline __int_seq
${eval __int_seq_inc = $$(call int_decode,$$1) $(value __int_seq)}
${eval __int_seq_dec = $(value __int_seq) $$(call int_decode,$$1)}

