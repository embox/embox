#
# Embox root Makefile.
#
# This file have just to setup proper make flags and to invoke mk/main.mk
# which will perform the real work.
#
# Author: Eldar Abusalimov
#

ifndef __mk_ready

# Check Make version (we need at least GNU Make 3.81). Fortunately,
# '.FEATURES' built-in variable has been introduced exactly in GNU Make 3.81.
ifneq ($(origin .FEATURES),default)
$(error Unsupported Make version. \
	Mybuild does not work properly with GNU Make $(MAKE_VERSION), \
	please use GNU Make 3.81 or above.)
endif

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --no-print-directory
MAKEFLAGS += --warn-undefined-variables

.DEFAULT_GOAL := all

.NOTPARALLEL :
% :
	@$(MAKE) __mk_ready=1 $@

else

include mk/load.mk

endif

