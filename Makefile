#
# Embox root Makefile.
#
# This file have just to setup proper make flags and to invoke mk/main.mk
# which will perform the real work.
#
# Author: Eldar Abusalimov
#

# Check Make version (we need at least GNU Make 3.81). Fortunately,
# '.FEATURES' built-in variable has been introduced exactly in GNU Make 3.81.
ifneq ($(origin .FEATURES),default)
$(error Unsupported Make version. \
	Mybuild does not work properly with GNU Make $(MAKE_VERSION), \
	please use GNU Make 3.81 or above.)
endif

# Disable everything, turn on undefined variables check.
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --no-print-directory
MAKEFLAGS += --warn-undefined-variables

# Fixup for case when prompt contains dollar signs.
# Avoids bogus 'undefined variable' warnings.
export PS1 :=

# Check for mcmodel compatibility before building
.PHONY: check-mcmodel
check-mcmodel:
	@bash ./scripts/check_mcmodel.sh || { echo "mcmodel check failed. Please check the error above and consider using -mcmodel=medany."; exit 1; }

# Make 'all' depend on check-mcmodel
all: check-mcmodel

.DEFAULT_GOAL := all

# Force multiple targets listed in the command line to run independently,
# even if -j option is enabled. This allows 'make -j clean all' to run
# properly, at the same time executing each target in parallel.
.NOTPARALLEL :

.PHONY : $(sort all $(MAKECMDGOALS))
$(sort all $(MAKECMDGOALS)) :

# To account for upper limit on stack size in Mac OS X
ifneq ($(shell uname -s), Darwin) 
	@ulimit -s 65536 && $(MAKE) -C $(dir $(lastword $(MAKEFILE_LIST))) -f mk/main.mk $@
else
	@ulimit -s $(shell ulimit -Hs) && $(MAKE) -C $(dir $(lastword $(MAKEFILE_LIST))) -f mk/main.mk $@
endif

