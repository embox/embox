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
#   TRAVERSE_CALLBACK = \
#       OBJS_ALL := $$(OBJS_ALL) $$(addprefix $(NODE_DIR)/,$(NODE_OBJS))
#   $(eval $(call TRAVERSE,$(SRC_DIR),node.mk,TRAVERSE_CALLBACK))
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
#  3. (optional) Expression to evaluate as a callback after processing the node
#                with the following interface:
      NODE_DIR      =$(1)# 1. Directory containing the node descriptor
      NODE_FILE     =$(2)# 2. Descriptor file name
      NODE_SUBDIRS  =$(3)# 3. Enabled sub-directories
      NODE_OBJS     =$(4)# 4. Enabled objects
      NODE_LIBS     =$(5)# 5. Enabled libraries
      NODE_TARGETS  =$(6)# 6. Additional targets
      NODE_DEPTH    =$(7)# 7. Call depth
#      (see TRAVERSE_CALLBACK_EXAMPLE as an example)
#
TRAVERSE =$(call traverse_process_node,$(1),$(2),$(3),.)

#
# The callback for TRAVERSE that traces its input arguments.
# Just to show how it can be implemented.
# Note that everything except variables with NODE_ prefix should be escaped
# (because of using 'eval' function, see 'make' manual).
#
define TRAVERSE_CALLBACK_EXAMPLE
  $$(info $(NODE_DEPTH)processing $(NODE_DIR)/$(NODE_FILE))
  $$(info $(NODE_DEPTH) subdirs : $(NODE_SUBDIRS))
  $$(info $(NODE_DEPTH) targets : $(NODE_TARGETS))
  $$(info $(NODE_DEPTH) objs    : $(NODE_OBJS))
  $$(info $(NODE_DEPTH) libs    : $(NODE_LIBS))
endef

#
# The main routine used for recursive processing of tree nodes.
#
# Params:
#  1. Directory containing the node descriptor to process
#  2. Descriptor file name
#  3. User callback
#  4. (internal) Call depth, root caller should pass 'non-white' string
#
define traverse_process_node

  # Clean everything before entering into user defined code.
  SUBDIRS-y :=
  OBJS-y    :=
  LIBS-y    :=
  TARGETS-y :=
  # Provide the node location.
  SELFDIR   :=$(1)

  # Go!
  include $(1)/$(2)

  # Expand SUBDIRS-y value to local variable.
  subdirs_y :=$$(SUBDIRS-y)

  # Invoke user callback.
  $$(eval $$(call $(3),$(1),$(2),$$(subdirs_y),$$(OBJS-y),$$(LIBS-y),$$(TARGETS-y),$(4)))

  # Perform recursive walking over sub-directories.
  # It's important to note that subdirs_y variable is expanded only once
  # and before entering child subroutines (which will overwrite this variable),
  # hence we have not to construct any stacks and so on.
  $$(foreach subdir,$$(subdirs_y),\
    $$(eval $$(call $(0),$(1)/$$(subdir),$(2),$(3),$(4) ))\
  )

  # Clean everything again.
  SUBDIRS-y :=
  OBJS-y    :=
  LIBS-y    :=
  TARGETS-y :=

endef

endif # _traverse_mk_
