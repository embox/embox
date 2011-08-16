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
# Internal GMSL stuff widely used by another utils.
#
# Author: John Graham-Cumming
# Author: Eldar Abusalimov
#

ifndef __gmsl_internals_mk
__gmsl_internals_mk := 1

include core/common.mk

# This is the GNU Make Standard Library version number as a list with
# three items: major, minor, revision

gmsl_version := 1 0 12

# Used to output warnings and error from the library, it's possible to
# disable any warnings or errors by overriding these definitions
# manually or by setting GMSL_NO_WARNINGS or GMSL_NO_ERRORS

__gmsl_name := GNU Make Standard Library
__gmsl_warning = $(warning $(__gmsl_name): $1)
__gmsl_error = $(error $(__gmsl_name): $1)

ifdef GMSL_NO_WARNINGS
__gmsl_warning :=
endif
ifdef GMSL_NO_ERRORS
__gmsl_error :=
endif

# If GMSL_TRACE is enabled then calls to the library functions are
# traced to stdout using warning messages with their arguments

ifdef GMSL_TRACE
__gmsl_tr1 = $(warning $0('$1'))
__gmsl_tr2 = $(warning $0('$1','$2'))
__gmsl_tr3 = $(warning $0('$1','$2','$3'))
else
__gmsl_tr1 :=
__gmsl_tr2 :=
__gmsl_tr3 :=
endif

# Standard definitions for true and false.  true is any non-empty string,
# false is an empty string. These are intended for use with $(if).

#
# 'true', 'false' and 'not' have already been defined in core/common.mk
#

# Figure out whether we have $(eval) or not (GNU Make 3.80 and above).
# If we do not then output a warning message,
# if we do then some functions will be enabled.

__gmsl_have_eval := $(false)
__gmsl_ignore := $(eval __gmsl_have_eval := $(true))

# If this is being run with Electric Cloud's emake then warn that
# their $(eval) support is incomplete.
ifdef ECLOUD_BUILD_ID
$(warning \
  You are using Electric Cloud's emake which has incomplete $$(eval) support)
__gmsl_have_eval := $(false)
endif

# See if we have $(lastword) (GNU Make 3.81 and above)

__gmsl_have_lastword := $(lastword $(false) $(true))

# See if we have native or and and (GNU Make 3.81 and above)

__gmsl_have_or := $(if $(filter-out undefined,  \
    $(origin or)),$(call or,$(true),$(false)))
__gmsl_have_and := $(if $(filter-out undefined, \
    $(origin and)),$(call and,$(true),$(true)))

ifneq ($(__gmsl_have_eval),$(true))
$(call __gmsl_warning,$ \
  GNU Make $(MAKE_VERSION) does not support $$(eval): some functions disabled)
endif

__gmsl_dollar := $$

#
# Function:  gmsl_compatible
# Arguments: List containing the desired library version number (maj min rev)
# Returns:   $(true) if this version of the library is compatible
#            with the requested version number, otherwise $(false)
#
gmsl_compatible = $(strip                                                 \
    $(if $(call >,$(word 1,$1),$(word 1,$(gmsl_version))),               \
        $(false),                                                         \
        $(if $(call <,$(word 1,$1),$(word 1,$(gmsl_version))),           \
            $(true),                                                      \
            $(if $(call >,$(word 2,$1),$(word 2,$(gmsl_version))),       \
                $(false),                                                 \
                $(if $(call <,$(word 2,$1),$(word 2,$(gmsl_version))),   \
                    $(true),                                              \
                    $(call <=,$(word 3,$1),$(word 3,$(gmsl_version))))))))

# Helper function that translates any GNU Make 'true' value (i.e. a
# non-empty string) to our $(true)
__gmsl_make_bool = $(make_bool)

# This results in __gmsl_space containing just a space
__gmsl_space := $(\s)

# This results in __gmsl_newline containing just a newline
__gmsl_newline := $(\n)

# This results in __gmsl_tab containing a tab
__gmsl_tab := $(\t)

endif # __gmsl_internals_mk
