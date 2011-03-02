
$_PACKAGE := embox.cmd

SUBDIRS := $(filter-out _template,$(SUBDIRS))

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include

# Util for adding new command.
# Usage example: make ac-xxx

.PHONY: add_cmd-% ac-%

__CMD_TEMPLATE = _template
__CMD_FILTER = sed 's/__cmd_name/$*/;s/@date/@date '`date +%d.%m.%Y`'/'

add_cmd-% ac-%:
	cat $(__CMD_TEMPLATE)/cmd.em | $(__CMD_FILTER) > $*.em
	cat $(__CMD_TEMPLATE)/cmd.c | $(__CMD_FILTER) > $*.c
	svn add $*.c $*.em

SELFDIR ?= .
include $(wildcard $(SELFDIR)/*.em)
