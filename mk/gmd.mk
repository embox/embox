# ----------------------------------------------------------------------------
#
# GNU Make Debugger (GMD)
#
# A simple debugger for GNU Make Makefiles.
#
# Copyright (c) 2005-2006 John Graham-Cumming
#
# This file is part of GMD
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# Neither the name of the John Graham-Cumming nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# ----------------------------------------------------------------------------

# The GMD uses functions from the GNU Make Standard Library which must
# be included first

include gmsl.mk

# Verify that we have at least GNU Make Standard Library version 1.0.0

ifneq ($(call gmsl_compatible,1 0 0),)

# ----------------------------------------------------------------------------
# __BREAKPOINT is the only macro that the user of the GMD needs to be
# aware of.  To set a breakpoint in a rule simple add the
# $(__BREAKPOINT) variable:
#
#     all:
#         $(__BREAKPOINT)
#
# This will cause a breakpoint when the all rule runs.
# ----------------------------------------------------------------------------
__BREAKPOINT = $(__BANNER)                                                   \
               $(eval __TERMINATE := $(false))                               \
               $(foreach __HISTORY,                                          \
                   $(__LOOP),                                                \
                   $(if $(__TERMINATE),,                                     \
                      $(eval __TERMINATE := $(__BREAK))))


# __LOOP is used to create a loop for the interactive prompt.  The
# number of entries in this variable determines the maximum number of
# commands that can be typed at any breakpoint before the Make will
# continue automatically.

__LOOP := 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32

# __PROMPT is used to write out the prompt when at a breakpoint and
# get a command and argument from the user.  Getting the command is
# done using the shell 'read' command.

__PROMPT = $(shell read -p "$(__HISTORY)> " CMD ARG ; echo $$CMD $$ARG)

# __DEBUG is used to interpret a command that has been typed by the
# user. __DEBUG is where the various interactive commands are handled.

__DEBUG =  $(eval __c = $(strip $1))                                          \
           $(eval __a = $(strip $2))                                          \
           $(if $(call seq,$(__c),c),                                         \
              $(true),                                                        \
              $(if $(call seq,$(__c),q),                                      \
                $(error Debugger terminated build),                           \
                $(if $(call seq,$(__c),v),                                    \
                  $(warning $(__a) has value '$($(__a))'),                    \
                  $(if $(call seq,$(__c),d),                                  \
                    $(warning $(__a) is defined as '$(value $(__a))'),        \
                    $(if $(call seq,$(__c),o),                                \
                      $(warning $(__a) came from $(origin $(__a))),           \
                        $(if $(call seq,$(__c),b),                            \
                          $(call __BP_SET,$(__a)),                            \
                        $(if $(call seq,$(__c),r),                            \
                          $(call __BP_UNSET,$(__a)),                          \
                        $(if $(call seq,$(__c),l),                            \
                          $(call __BP_LIST),                                  \
                      $(if $(call seq,$(__c),h),                              \
                        $(warning c:     continue)                            \
                        $(warning q:     quit)                                \
                        $(warning v VAR: print value of $$(VAR))              \
                             $(warning o VAR: print origin of $$(VAR))        \
                             $(warning d VAR: print definition of $$(VAR))    \
                             $(warning b TAR: set a breakpoint on target TAR) \
                             $(warning r TAR: unset breakpoint on target TAR) \
                             $(warning l:     list all target breakpoints),   \
                             $(warning Unknown command '$(__c)'))))))))))

# Called by the __BREAKPOINT macro to prompt the user for a single
# command (using __PROMPT) and then handle the command using __DEBUG.

__BREAK = $(eval __INPUT := $(__PROMPT))                                      \
          $(call __DEBUG,                                                     \
              $(word 1,$(__INPUT)),                                           \
              $(word 2,$(__INPUT)))

# __BANNER prints the banner information output when a breakpoint is
# hit

__BANNER = $(warning GNU Make Debugger Break)                                 \
           $(if $@,                                                           \
             $(if $^,                                                         \
               $(warning - Building '$@' from '$^'),                          \
               $(warning - Building '$@'))                                    \
             $(if $<,$(warning - First prerequisite is '$<'))                 \
             $(if $%,$(warning - Archive target is '$%'))                     \
             $(if $?,$(warning - Prequisites '$?' are newer than '$@')))

# This is a list of target names that are set and unset by dynamic
# breakpoints.  Dynamic breakpoints are handled by modifying SHELL
# (which is expanded just before running any commands), check $@ to
# see if it's in the breakpoint list, and if it is __BREAKPOINT is
# called.

__BREAKPOINTS := $(empty_set)

# These functions are used inside the GMD to set, unset and list
# breakpoints.

__BP_SET =   $(eval __BREAKPOINTS := $(call set_insert,$1,$(__BREAKPOINTS)))  \
             $(warning Breakpoint set on `$1')

__BP_UNSET = $(if $(call set_is_member,$1,$(__BREAKPOINTS)),                  \
               $(eval __BREAKPOINTS := $(call set_remove,$1,$(__BREAKPOINTS)))\
               $(warning Breakpoint on `$1' removed),                         \
               $(warning Breakpoint on `$1' not found))

__BP_LIST = $(if $(__BREAKPOINTS),                                            \
              $(warning Current target breakpoints:                           \
                  $(addsuffix ',$(addprefix `,$(__BREAKPOINTS)))),            \
              $(warning No target breakpoints set))

# This function is used to see if the target currently being built is
# in the breakpoint list and if it is hit the breakpoint It does two
# interesting things: firstly it sets the __BP_FLAG variable to be the
# name of the target currently being built (this is checked below and
# used to prevent recursion into __BP_CHECK again in __BP_NEW_SHELL);
# secondly it prevents a warning about SHELL referencing itself with
# the peculiar construct $(call SHELL,__BREAKPOINT)---a side effect of
# doing $(call FOO) is that FOO's recursive expansion flag gets reset.

__BP_CHECK = $(if $(call set_is_member,$@,$(__BREAKPOINTS)),                  \
               $(eval __BP_FLAG := $@)                                        \
               $(eval __IGNORE := $(call SHELL,__BREAKPOINT)))

__BP_FLAG :=

# Store the current value of the SHELL and then redefine SHELL so that
# it calls __BP_NEW_SHELL to check to see if the __BP_FLAG is set (in
# which case a breakpoint is hit), if it's not set the __BP_CHECK is
# called to see if the breakpoint should be hit (which will happen
# when SHELL is called again with an argument of __BREAKPOINT).  So
# setting the __BP_FLAG prevents the breakpoint from being checked
# more than once per target and only __BP_CHECK will set $1 and hence
# cause a breakpoint. (SHELL also uses the $(call FOO) trick mentioned
# above to prevent __BP_NEW_SHELL from getting a recursive definition
# warning).

__BP_OLD_SHELL := $(SHELL)
__BP_NEW_SHELL = $(if $(call seq,$(__BP_FLAG),$@),                            \
                     $(call $1,),                                             \
                     $(__BP_CHECK))$(__BP_OLD_SHELL)
SHELL = $(call __BP_NEW_SHELL,$1)

# The GMD version number

__GMD_VERSION := 1 0 2

else

$(error GMD cannot load because GMSL version is not 1.0.0 or higher)

endif
