#
#   Date: Mar 3, 2014
# Author: Anton Kozlov
#

include mk/script/script-common.mk

ifeq ($(MK_PLUGIN_NAME),)
$(error MK_PLUGIN_NAME is not set.)
endif

$(info #include <QtPlugin>)
$(info )
$(info Q_IMPORT_PLUGIN($(MK_PLUGIN_NAME)))

