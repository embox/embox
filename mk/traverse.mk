#
# Yet another implementation of non-recursive 'make'.
#
# Copyright (C) 2009 Eldar Abusalimov. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

ifndef _traverse_mk_
_traverse_mk_:=1

#
# Usage:
#
# Part of root Makefile using TRAVERSE may look like:
##
#   include $(MK_DIR)/traverse.mk
#
#   define traverse_callback
#     DIRS_ALL:=$(DIRS_ALL) $(NODE_DIR)
#     OBJS_ALL:=$(OBJS_ALL) $(addprefix $(NODE_DIR)/,$(OBJS-y))
#     LIBS_ALL:=$(LIBS_ALL) $(addprefix $(NODE_DIR)/,$(LIBS-y))
#     NODE_RESULT:=$(SUBDIRS-y)
#   endef
#
#   traverse_vars:=SUBDIRS-y OBJS-y LIBS-y
#
#   $(eval $(call TRAVERSE,$(SRC_DIR),node.mk,traverse_callback,traverse_vars))
#
#   all: $(OBJS_ALL)
#   	@$(CC) $(LDFLAGS) -T $(LDSCRIPT) -o $(TARGET) $(OBJS_ALL)
##
#
# node.mk files placed in each sub-directory should contain something like:
##
#   SUBDIRS-y              += irq
#   SUBDIRS-$(CONFIG_TEST) += test
#
#   OBJS-y                 += main.o
#   OBJS-y                 += init.o
#   OBJS-$(CONFIG_TIMER)   += timer.o
#
#   LIBS-y                 += lib.a
##
# Each time before node.mk is included variables SUBDIRS-y, OBJS-y, and LIBS-y
# are set to empty string, and SELFDIR contains location of node.mk to be
# processed.
#
# Enjoy!
#

#
# Walks the directory tree starting at the specified root,
# processing node descriptors with given file name
# and invoking user-defined callback (if any).
#
# Params:
#  1. Root directory
#  2. File name of node descriptor containing info about subdirs and objects
#  3. Expression to evaluate as a callback after processing the node
#                with the following interface:
#      [in] NODE_DIR    Directory containing the node descriptor
#      [in] NODE_FILE   Descriptor file name
#      [in] NODE_DEPTH  Call depth
#     [out] NODE_RESULT Callback should return list of directories to continue
#                       the walking by assigning it to this variable
#    (see TRAVERSE_CALLBACK_EXAMPLE as an example)
#  4. List of variable names to clear before entering each node
#
TRAVERSE =$(eval $(call traverse_process_node,$(1),$(2),$(3),$(4),.))

#
# The callback for TRAVERSE that traces its input arguments.
# Just to show how it can be implemented.
#
define TRAVERSE_CALLBACK_EXAMPLE
  $(info $(NODE_DEPTH)processing $(NODE_DIR)/$(NODE_FILE))
  $(info $(NODE_DEPTH) subdirs : $(SUBDIRS-y))
  $(info $(NODE_DEPTH) objs    : $(OBJS-y))
  $(info $(NODE_DEPTH) libs    : $(LIBS-y))
  NODE_RESULT:=$(SUBDIRS-y)
endef

#
# The main routine used for recursive processing of tree nodes.
#
# Params:
#  1. Directory containing the node descriptor to process
#  2. Descriptor file name
#  3. User callback
#  4. List of variable names to clear before entering each node
#  5. (internal) Call depth, root caller should pass 'non-white' string
#
define traverse_process_node
  ifeq ($(wildcard $(1))),)
  $$(error Traverse error: Node not found: $(1))
  endif
  ifeq ($(wildcard $(1)/$(2)),)
  $$(error Traverse error: Node descriptor not found: $(1)/$(2))
  endif

  # Clean everything before entering into user defined code.
  $$(foreach var_name,$($(4)),\
    $$(eval $$(var_name):=)\
  )

  # Provide the node location.
  SELFDIR   :=$(1)

  # Go!
  include $(1)/$(2)

  # Invoke user callback.
  NODE_DIR    :=$(1)
  NODE_FILE   :=$(2)
  NODE_DEPTH  :=$(5)
  NODE_RESULT :=
  $$(eval $$(value $(3)))

  # Perform recursive walking over sub-directories.
  # It's important to note that subdirs_y variable is expanded only once
  # and before entering child subroutines (which will overwrite this variable),
  # hence we have not to construct any stacks and so on.
  $$(foreach subdir,$$(NODE_RESULT),\
    $$(eval $$(call $(0),$(1)/$$(subdir),$(2),$(3),$(4),$(5) ))\
  )

  # Clean everything again.
  $$(foreach var_name,$($(4)),\
    $$(eval $$(var_name):=)\
  )

endef

endif # _traverse_mk_
