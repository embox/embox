#
# Author: Eldar Abusalimov
#

# Disable some annoying builtin rules. These speeds up the build a bit.
.SUFFIXES:
% : RCS/%
% : %,v
% : RCS/%,v
% : %,w
% : RCS/%,w
% : s.%
% : SCCS/s.%
% : %.w
%.c : %.y
%.c : %.l
%.c : %.w
