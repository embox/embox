
$_PACKAGE := embox.cmd

SUBDIRS := $(filter-out _cmd_template,$(SUBDIRS))

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include

# Util for adding new command.
# Usage example: CMD_NAME=cmd make ac

.PHONY: help

_CMD_TEMPLATE = _cmd_template
FILTER = sed 's/<CMD_NAME>/$(CMD_NAME)/;s/<REQ_MODS>/$(REQ_MODS)/;s/cmd_template/$(CMD_NAME)/'

add_cmd ac:
	@[[ -d $(CMD_NAME) ]] || mkdir -p $(CMD_NAME)
	cat $(_CMD_TEMPLATE)/Makefile_template | $(FILTER) > $(CMD_NAME)/Makefile
	cat $(_CMD_TEMPLATE)/cmd_template.c | $(FILTER) > $(CMD_NAME)/$(CMD_NAME).c
	cat $(_CMD_TEMPLATE)/cmd_template_help.inc | $(FILTER) > $(CMD_NAME)/$(CMD_NAME)_help.inc
	svn add $(CMD_NAME)
