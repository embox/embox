#
# Common rules of compiling the EMBOX sources.
#
# Author: Eldar Abusalimov
#

# Disable some builtin rules. These speeds up the build a bit.
.SUFFIXES:
% : RCS/%
% : %,v
% : RCS/%,v
% : %,w
% : RCS/%,w
% : s.%
% : SCCS/s.%

%.c : %.y
%.c : %.l
%.c : %.w

%.o:%.c
	$(CC) $(CCFLAGS) $(CCINCLUDES) -MMD -c -o $@ $<

%.o:%.S
	$(CC) $(CCFLAGS) $(CCINCLUDES) -MMD -c -o $@ $<
