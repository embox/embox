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
#   # Guard symbol used in subdirs.
#   _ = EMBUILD/$(abspath $(dir))/
#
#   # Traverse always defines SELFDIR before entering sub-makefile.
#   dir = $(SELFDIR)
#   DIRS := $(call TRAVERSE,$(SRC_DIR),node.mk)
#
#   # Get list of all objects.
#   OBJS := $(foreach dir,$(DIRS),$(wildcard $($_OBJS:%=$(dir)/%)))
#
#   all: $(OBJS)
#   	@$(CC) $(LDFLAGS) -T $(LDSCRIPT) -o $(TARGET) $(OBJS)
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
# be overridden by setting $_SUBDIRS variable in any node.mk file, e.g.:
#   $_SUBDIRS := $(filter-out include,$($_SUBDIRS))
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
TRAVERSE= \
  $(eval __traverse_return:=) \
  $(eval $(call __traverse_invoke,$1,$2)) \
  $(__traverse_return)

#
# A kind of wrapper for __traverse_process that allows us to use unescaped code
# and human-readable varible names in the latter. Also incapsulates recursive
# sub-routine invocation code.
#
# Params:
#  1. Directory containing the node descriptor to process
#  2. Descriptor file name
#
define __traverse_invoke

  # We use such long prefixed name to prevent global namespace pollution.
  __traverse_node_dir  :=$1
  __traverse_node_file :=$2
  $(value __traverse_process)

  # Perform recursive walking over sub-directories.
  # It's important to note that __traverse_process_result variable is expanded
  # only once and before entering child subroutines (which will overwrite this
  # variable), hence we have not to construct any stacks and so on.
  $$(foreach subdir,$$(__traverse_process_result),$$(eval \
    $$(call __traverse_invoke,$1/$$(subdir),$2)\
  ))

endef

#
# Param: subdirectories list relative to __traverse_node_dir
# possibly containing wildcard expressions.
#
# In a nutshell:
#   Expand wildcards.
#   Leave only directories (they contain trailing slash).
#   Get back to sub-dirs relative names and remove duplicates.
#
__traverse_subdirs_wildcard = \
  $(sort \
    $(patsubst $(__traverse_node_dir)/%/,%, \
      $(filter $(__traverse_node_dir)/%/, \
        $(wildcard $(1:%=$(__traverse_node_dir)/%/)) \
      ) \
    ) \
  )

#
# The main routine used for recursive processing of tree nodes.
#
define __traverse_process
 # Check input arguments
 ifeq ($(wildcard $(__traverse_node_dir)),)
  $(warning EMBuild traverse warning $N \
    Node not found: $(__traverse_node_dir))
    Skipping)
  __traverse_process_result :=
 else

  # Append current node to the resulting node list.
  __traverse_return += $(__traverse_node_dir)

  # Provide the node location.
  SELFDIR := $(__traverse_node_dir)
  SELF    := $(notdir $(SELFDIR))

  # Sometimes it is useful to define variables as recursively expanded.
  $_SELFDIR := $(SELFDIR)

  # Default to expansion of *.
  $_SUBDIRS := $(call __traverse_subdirs_wildcard,*)

  ifneq ($(and $(__traverse_node_file), \
      $(wildcard $(__traverse_node_dir)/$(__traverse_node_file))),)
    # Go!
    include $(__traverse_node_dir)/$(__traverse_node_file)
  else ifneq ($(wildcard $(__traverse_node_dir)/Makefile),)
    include $(__traverse_node_dir)/Makefile
  else ifneq ($(wildcard $(__traverse_node_dir)/makefile),)
    include $(__traverse_node_dir)/makefile
  else
    $(warning EMBuild traverse warning:: \
      Node descriptor not found in $(__traverse_node_dir): $N \
      neither $(if $(__traverse_node_file), \
        $(__traverse_node_file) nor) \
      $(if $(filter Makefile,$(__traverse_node_file)),,Makefile nor) \
      $(if $(filter makefile,$(__traverse_node_file)),,makefile) \
      does not exist $N \
      Skipping)
  endif

  # Prepare return value.
  __traverse_process_result := $(call __traverse_subdirs_wildcard,$($_SUBDIRS))

 endif
endef

endif # _traverse_mk_
