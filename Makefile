#
# Embox root Makefile.
#
# This file have just to setup proper make flags and to invoke mk/main.mk
# which will perform the real work.
#
# Author: Eldar Abusalimov
#

MK_DIR := $(abspath mk)

ifdef __mk_ready

include mk/load.mk

else

MAKEFLAGS += --include-dir=$(MK_DIR)
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --no-print-directory
MAKEFLAGS += --warn-undefined-variables

.DEFAULT_GOAL := all

% :
	@$(MAKE) __mk_ready=1 $@

endif

