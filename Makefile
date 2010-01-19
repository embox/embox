#
# EMBOX root Makefile
#
# This file just have to define ROOT_DIR variable and to include maik.mk
# which will perform the real work.
#
# Author: Eldar Abusalimov
#

ROOT_DIR:=.#$(CURDIR)

include $(ROOT_DIR)/mk/main.mk
