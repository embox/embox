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

# Some parts of this file are derived from GMSL.

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
# Associative arrays.
#
# Author: Eldar Abusalimov
#

# XXX review or drop out. -- Eldar

ifndef __util_map_mk
__util_map_mk := 1

#
# Function:  map_put
# Arguments: 1: Name of associative array
#            2: The key value to associate
#            3: The value associated with the key
# Returns:   None
#
map_put = $(strip \
  $(call assert_called,map_put,$0) \
  $(__map_args_check) \
  $(call var_assign_simple,$(__map_entry),$3) \
)

#
# Function:  map_get
# Arguments: 1: Name of associative array
#            2: The key to retrieve
# Returns:   The value stored in the array for that key
#
map_get = $(strip \
  $(call assert_called,map_get,$0) \
  $(__map_args_check) \
  $(if $(filter-out undefined,$(origin $(__map_entry))),$($(__map_entry))) \
)

#
# Function:  map_remove
# Arguments: 1: Name of associative array
#            2: The key value to remove
# Returns:   None
#
map_remove = $(strip \
  $(call assert_called,map_remove,$0) \
  $(__map_args_check) \
  $(call var_undefine,$(__map_entry),$3) \
)

#
# Function:  keys
# Arguments: 1: Name of associative array
# Returns:   Returns a list of all defined keys in the array
#
map_keys = $(sort \
  $(call assert_called,keys,$0) \
  $(call assert,$(strip $1),Must specify map name) \
  $(patsubst $(call __map_entry,$1,%),%, \
    $(filter $(call __map_entry,$1,%),$(.VARIABLES)) \
)

#
# Function:  defined
# Arguments: 1: Name of associative array
#            2: The key to test
# Returns:   Returns true if the key is defined (i.e. not empty)
#
map_defined = $(__gmsl_tr2)$(call assert_no_dollar,$0,$1$2)$(call sne,$(call get,$1,$2),)

__map_args_check = \
  $(call assert,$(and $(strip $1),$(strip $2)),Must specify map and key names)

__map_entry = \
  $(__map_entry_prefix)$(call dollar_encode,$1)_$$_$(call dollar_encode,$2)
__map_entry_prefix := __map_entry_

endif # __util_map_mk
