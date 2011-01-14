#
# Wildcard derivatives.
#
# Author: Eldar Abusalimov
#

ifndef __util_wildcard_mk
__util_wildcard_mk := 1

include util/common.mk

##
# Function: r-wildcard
#
# Extended version of wildcard that understands "double asterisk" pattern (**).
#
# Params:
#  1. Patterns containing wildcard characters (possibly including **)
#
# Returns: Wildcard expansion of the patterns.
#
# Usage: $(call r-wildcard,pattern...)
#
# Note: does not handle properly more than one ** tokens in single pattern.
#
r-wildcard = \
  $(if $(call list_single,$1),$ \
    $(call __r-wildcard,$(subst **,* *,$1)),$ \
    $(foreach token,$(call $0,$(token)))$ \
  )# Split argument and recall self for each single pattern.

# Accepts single pattern with "**" replaced by "* *",
# performs some checks and prepares the arguments for __r-wildcard-expand.
__r-wildcard = \
  $(if $(call list_single,$1),$(wildcard $1),$(if $(filter 2,$(words $1)),$ \
    $(call __r-wildcard-expand,$ \
        $(patsubst %*,%,$(word 1,$1)),*,$(patsubst *%,%,$(word 2,$1)),),$ \
    $(error Handling more than one ** tokens is not implemented)$ \
  ))

# Recursively performs wildcard expansion of double asrerisk for *, */*, ...
#
# Some examples:
#  (good) foo/*/bar
#      -> (single word after replacement, use simple wildcard)
#  (good) foo/**/bar
#      -> foo/* */bar
#      -> foo/, *, /bar
#      -> foo/, */*, /bar
#      -> ... (expand wildcards while foo/*/*/.../ gives non-empty result)
#  (bad)  foo/**/bar/**
#      -> foo/* */bar/* *
#      -> (3 words, fail with an error)
__r-wildcard-expand = $4 \
  $(if $(filter %/,$(wildcard $1$2/)),$(call $0,$1,$2/*,$3,$(wildcard $1$2$3)))

# Directory/file versions of wildcard.
# Both of them are based on the fact that wildcard expansion of the expression
# containing the trailing slash drops the slash for files but leaves it for
# directories.
#
# TODO some strange behavior, check again. -- Eldar

##
# Function: d-wildcard
#
# Directory-only wildcard. This version of wildcard filters out any files
# leaving only directories.
#
# Params:
#  1. Patterns containing wildcard characters
#
# Returns: Wildcard expansion of the patterns (directories only).
#
# Usage: $(call d-wildcard,pattern...)
#
d-wildcard = $(patsubst %/,%,$(filter %/,$(wildcard $(1:%=%/))))

##
# Function: f-wildcard
#
# File-only wildcard. This version of wildcard leaves only files in the
# expansion result.
#
# Params:
#  1. Patterns containing wildcard characters
#
# Returns: Wildcard expansion of the patterns (files only).
#
# Usage: $(call f-wildcard,pattern...)
#
f-wildcard = $(filter-out $(call d-wildcard,$1),$(wildcard $1))

##
# Function: wildcard_first
#
# Tries to expand each pattern sequentially until getting non-empty expansion.
#
# Params:
#  1. Patterns containing wildcard characters
#
# Returns: The first non-empty wildcard expansion.
#
# Usage: $(call wildcard_first,pattern...)
#
wildcard_first = $(call __wildcard_first,  wildcard,$1)

##
# Function: d-wildcard_first
#
# Directory-only version of wildcard_first.
#
# See: wildcard_first, d-wildcard
#
d-wildcard_first = $(call __wildcard_first,d-wildcard,$1)

##
# Function: f-wildcard_first
#
# File-only version of wildcard_first.
#
# See: wildcard_first, f-wildcard
#
f-wildcard_first = $(call __wildcard_first,f-wildcard,$1)

# Expand each pattern (arg 2) one by one sequentially using proper wildcard
# version (arg 1) until getting non-empty expansion.
__wildcard_first = $(if $2,$ \
  $(or $(call $1,$(firstword $2)),$(call $0,$1,$(wordlist 2,$(words $2),$2))))

#
# TODO: all functions below are not used, may be drop them? -- Eldar
#

##
# Function: wildcard_relative
#
# Performs wildcard expansion within the specified base directory.
#
# Params:
#  1. Base directory
#  2. Wildcard patterns relative to the base directory
#
# Returns: Wildcard expansion of the pattern within the base directory.
#
# Usage: $(call wildcard_relative,base,pattern...)
#
# TODO: this will not for absolute paths, may be drop it at all? -- Eldar
#
  wildcard_relative = $(call __wildcard_relative,  wildcard,$1,$2)
d-wildcard_relative = $(call __wildcard_relative,d-wildcard,$1,$2)
f-wildcard_relative = $(call __wildcard_relative,f-wildcard,$1,$2)

# Expand wildcards (first argument) for patterns within the base dir
# and get back to relative names.
__wildcard_relative = \
  $(foreach basedir,$(abspath $2),$ \
    $(patsubst $(basedir)/%,%,$(call $1,$(3:%=$(basedir)/%)))$ \
  )

d-wildcard_relative_first = $(call __wildcard_relative,d-wildcard_first,$1,$2)
f-wildcard_relative_first = $(call __wildcard_relative,f-wildcard_first,$1,$2)
  wildcard_relative_first = $(call __wildcard_relative,  wildcard_first,$1,$2)

endif # __util_wildcard_mk
