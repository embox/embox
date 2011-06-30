# This file is derived from Embox test template.

##
# In general you have not to edit this Makefile.

##
# To include the test just add the following directive
# to mods-tests.conf (or to mods.conf if the first does not exist):

## For the regular platform-independent test:
# test(<test_name>)
#
## For the test placed in the ./platform directory:
# test_platform(<test_name>)
#

##
# By default the test is named as the container directory, includes all *.c
# source files and does not depend on other mods.
# To override this behavior please uncomment the following three lines
# and fill $_test_name, $_test_srcs and $_test_deps variables:

# $_test_name :=
# $_test_srcs :=
# $_test_deps :=

##
# Default values. Do not edit.
$_test_name ?= $(notdir $($_SELFDIR))
$_test_srcs ?= *.c
$_test_deps = core.kernel.timer core.kernel.thread.core

$_MODS += $($_test_name)
$_SRCS-$($_test_name) += $($_test_srcs)
$_DEPS-$($_test_name) += $($_test_deps)
