#
# Copyright 2008-2010, Mathematics and Mechanics faculty
#                   of Saint-Petersburg State University. All rights reserved.
# Copyright 2008-2010, Lanit-Tercom Inc. All rights reserved.
#
# Copyright 2005-2010 John Graham-Cumming
#
# Please see COPYRIGHT for details.
#

#
# GMSL frontend.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_mk
__gmsl_mk := 1

include gmsl/internals.mk

include gmsl/logic.mk
include gmsl/list.mk
include gmsl/string.mk
include gmsl/set.mk
include gmsl/math.mk
include gmsl/map.mk
include gmsl/stack.mk
include gmsl/debug.mk

endif # __gmsl_mk

