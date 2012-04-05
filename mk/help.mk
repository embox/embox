#
#   Base support for help sections in main build scripts
#
#   Date: Apr 5, 2012
# Author: Anton Kozlov
#

help_template = \
=== Build Help : $1 ===$(\n)$(\n)$2$(\n)$(\n)USAGE$(\n)$(\n)$3

help_section = $(\n)$(\n)--- $1 ---$(\n)

help_point:= $(\n)$(\n)*
help_pointed_list = $(subst *,$(help_point),$1)$(\n)
