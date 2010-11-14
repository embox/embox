#
# Wildcard derivatives.
#
# Author: Eldar Abusalimov
#

#include util/core.mk

##
# Function: r-wildcard
#
# Extended version of wildcard that understands "double asterisk" pattern (**).
#
# Usage: $(call r-wildcard,pattern...)
#
# Note: does not handle properly more than one ** tokens in single pattern.
#
r-wildcard = \
  $(if $(filter 1,$(words $1)),$ \
    $(call __r-wildcard,$(subst **,* *,$1)),$ \
    $(foreach token,$(call $0,$(token)))$ \
  )# Split argument and recall self for each single pattern.

# Accepts single pattern with "**" replaced by "* *",
# performs some checks and prepares the arguments for __r-wildcard-expand.
__r-wildcard = \
  $(if $(filter 1,$(words $1)),$(wildcard $1),$(if $(filter 2,$(words $1)),$ \
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

##
# Function: d-wildcard
#
# Directory-only wildcard. This version of wildcard filters out any files
# leaving only directories.
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
# Usage: $(call f-wildcard,pattern...)
#
f-wildcard = $(patsubst %/,%,$(filter-out %/,$(wildcard $(1:%=%/))))

#
# XXX docs were copy-pasted from traverse.mk -- Eldar
#
# Params:
#  1. Base directory
#  2. Subdirectories list relative to the base dir possibly containing wildcard
#     expressions.
#
# In a nutshell:
#   Expand d-wildcards for sub-dirs within the base dir.
#   Get back to sub-dirs relative names and remove duplicates.
#
d-wildcard_relative = $(call __wildcard_relative,d-wildcard,$1,$2)
f-wildcard_relative = $(call __wildcard_relative,f-wildcard,$1,$2)
  wildcard_relative = $(call __wildcard_relative,  wildcard,$1,$2)
__wildcard_relative = $(__gmsl_tr3)$(foreach basedir,$(abspath $2),$ \
  $(patsubst $(basedir)/%,%,$(call $1,$(3:%=$(basedir)/%))))

d-wildcard_first = $(call __wildcard_first,d-wildcard,$1)
f-wildcard_first = $(call __wildcard_first,f-wildcard,$1)
  wildcard_first = $(call __wildcard_first,  wildcard,$1)
__wildcard_first = $(__gmsl_tr2)$(or $(call $1,$(firstword $2)),$ \
                        $(call $0,$1,$(call rest,$2)))

d-wildcard_relative_first = $(call __wildcard_relative,d-wildcard_first,$1,$2)
f-wildcard_relative_first = $(call __wildcard_relative,f-wildcard_first,$1,$2)
  wildcard_relative_first = $(call __wildcard_relative,  wildcard_first,$1,$2)

