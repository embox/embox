#
# Some useful stuff lives here.
#
# Author: Eldar Abusalimov
#

ifndef _util_mk_
_util_mk_ := 1

include core/common.mk
include core/string.mk
include gmsl/gmsl.mk # agrhhh!.. avoid using it. -- Eldar

# Make-style error and warning strings.

# The most general way to get error/warning string.
# First argument should contain the location to print (directory and file).
error_str_file   = $1:1: error:
warning_str_file = $1:1: warning:

# Print location using the first argument as directory
# and 'Makefile' as file within the directory.
error_str_dir    = $(call error_str_file,$1/Makefile)
warning_str_dir  = $(call warning_str_file,$1/Makefile)

# Generates error/warning string in $(dir)/Makefile.
error_str        = $(call error_str_dir,$(dir))
warning_str      = $(call warning_str_dir,$(dir))

endif # _util_mk_
