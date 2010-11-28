#
# Yet another implementation of non-recursive 'make'.
#
# Copyright (C) 2009-2010 Eldar Abusalimov. All rights reserved.
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

ifndef $(already_included)

include util/file.mk

#
# Usage:
#
# Part of root Makefile using traverse may look like:
##
#   include $(MK_DIR)/traverse.mk
#
#   # Guard symbol used in subdirs.
#   _ = traversed/$(abspath $(dir))/
#
#   # Traverse always defines SELFDIR before entering sub-makefile.
#   dir = $(SELFDIR)
#   DIRS := $(call traverse,$(SRC_DIR),node.mk)
#
#   # Get list of all objects.
#   OBJS := $(foreach dir,$(DIRS),$(wildcard $($_OBJS:%=$(dir)/%)))
#
#   all: $(TARGET)
#   $(TARGET): $(OBJS)
#   	@$(CC) $(LDFLAGS) -o $@ $^
##
#
# node.mk files placed in each sub-directory should contain something like:
##
#   $_OBJS += irq.o
#   $_OBJS += main.o
#   $_OBJS += init.o
##
# Each time before node.mk is included variable SELFDIR is set to location of
# node.mk being processed. Guard symbol ($_) is used to prevent namespace
# collisions, and it also helps to determine where did a variable come from.
#
# By default traverse searches the whole directory tree, but this behavior can
# be overridden by setting SUBDIRS variable in any node.mk file, e.g.:
#   SUBDIRS := $(filter-out include,$(SUBDIRS))
# This expression will exclude sub-directory named "include" from searching.
#
# Enjoy!
#

#
# Walks the directory tree starting at the specified root,
# processing node descriptors with given file name.
#
# Params:
#  1. Root directory
#  2. (optional) File name of node descriptor containing info about subdirs
#   If not specified traverse will search for files named Makefile and makefile
#
traverse = $(strip \
  $(call assert_called,traverse,$0) \
  $(foreach __traverse_root,$(call d-wildcard,$1), \
    $(call __traverse_invoke,$(__traverse_root),$2) \
  ) \
)

#
# A kind of wrapper for __traverse_process that allows us to use unescaped code
# and human-readable varible names in the latter. Also incapsulates recursive
# sub-routine invocation code.
#
# Params:
#  1. Directory containing the node descriptor to process
#  2. Descriptor file name
#
__traverse_invoke = $1 \
  $(foreach subdir,$(__traverse_process),$(call $0,$1/$(subdir),$2))

__traverse_process = \
  ${eval $(value __traverse_process_mk)}$(__traverse_process_result)

#
# The main routine used for recursive processing of tree nodes.
#
define __traverse_process_mk
  # Provide the node location.
  SELFDIR := $1
  # Sometimes it is useful to define variables as recursively expanded.
  $_SELFDIR := $(SELFDIR)# TODO try to avoid it

  # Default SUBDIRS to expansion of *.
  __traverse_subdirs := $(call d-wildcard_relative,$1,*)
  SUBDIRS := $(__traverse_subdirs)

  __traverse_include := \
    $(call f-wildcard_first,$(addprefix $1/,$2 Makefile makefile))
  ifneq ($(__traverse_include),)
    # Go!
    include $(__traverse_include)
  else
    $(info $(call error_str_file,$(__traverse_parent_node_file)) \
      Node file not found in subdirectory $1: \
      neither $(if $2,$2 nor \
      )$(if $(filter Makefile,$2),,Makefile nor \
      )$(if $(filter makefile,$2),,makefile \
      )does not exist. Skipping)
    # TODO defer error processing. -- Eldar
#    $(error Error traversing source tree)
    SUBDIRS :=
  endif

  # Prepare return value.
  __traverse_process_result := \
    $(filter $(__traverse_subdirs),$(call d-wildcard_relative,$1,$(SUBDIRS)))

endef

__traverse_parent_node_file = $(strip \
  $(patsubst $(abspath $(__traverse_root))/%,$(__traverse_root)/%, \
    $(abspath $(call f-wildcard_first,$(addprefix $1/../,$2 Makefile makefile))) \
  ) \
)

endif # $(already_included)
