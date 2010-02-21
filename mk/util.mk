#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#

ifndef _util_mk_
_util_mk_ := 1

#
# r-patsubst stands for recursive patsubst.
# This function has the same syntax as patsubst, excepting that you should use
# $(call ...) to invoke it.
# Unlike regular patsubst this one performs pattern replacement until at least
# one of the words in target expression matches the pattern.
#
# For example:
#   $(call r-patsubst,%/,%,foo/ bar/// baz) produces "foo bar baz"
#   whilst $(patsubst %/,%,foo/ bar/// baz) is just  "foo bar// baz"
#
# Usage: $(call r-patsubst,pattern,replacement,text)
#
r-patsubst = $(if $(filter $1,$3),$(call $0,$1,$2,$(3:$1=$2)),$3)

#
# Extended version of wildcard that understands double asterisk pattern (**).
#
# Usage: $(call r-wildcard,pattern)
#
# NOTE: does not handle properly more than one ** tokens.
#
r-wildcard = $(strip $(call __r-wildcard,$1,$(wildcard $1)))

__r-wildcard = \
  $(if $(and $(findstring **,$1),$2), \
    $(call $0,$(subst **,**/*,$1),$(wildcard $(subst **,**/*,$1))) \
  ) $2


# Make-style error and warning strings.

# The most general way to get error/warning string.
# First argument should contain the location to print (directory and file).
error_str_file   = $1:1: EMBuild error::
warning_str_file = $1:1: EMBuild warning::

# Print location using the first argument as directory
# and 'Makefile' as file within the directory.
error_str_dir    = $(call error_str_file,$1/Makefile)
warning_str_dir  = $(call warning_str_file,$1/Makefile)

# Generates error/warning string in $(dir)/Makefile.
error_str        = $(call error_str_dir,$(dir))
warning_str      = $(call warning_str_dir,$(dir))

# Just for better output readability.
define N


endef

endif # _util_mk_
