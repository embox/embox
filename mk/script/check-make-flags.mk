#
#   Date: Feb 21, 2012
# Author: Eldar Abusalimov
#

ifeq ($(filter --no-print-directory,$(MAKEFLAGS)),)
$(error '--no-print-directory' flag must be specified)
endif

ifneq ($(findstring d,$(lastword $(MAKEFLAGS))),)
$(error '-d' flag must not be used. \
	Also note that '--debug=FLAGS' should be avoided too, moreover, \
	the script can't detect the presense of the latter, \
	but building will fail later anyway)
endif
