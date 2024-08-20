#!/bin/sh

set -eu

MODULE_ID=$1
SOURCE_OBJ=$2
TARGET_OBJ=$3

if [ ! $MODULE_ID ]; then
	MODULE_ID=$TARGET_APP
fi

if [ ! TARGET_OBJ ]; then
	TARGET_OBJ=$FILE_APP
fi

if [ ! SOURCE_OBJ ]; then
	SOURCE_OBJ=$(dirname $TARGET_OBJ)/$(basename $TARGET_OBJ | sed 's/strip\.//')
fi

if [ ! $MODULE_ID ] || [ ! $TARGET_OBJ ] || [ ! $SOURCE_OBJ ]; then
	echo MODULE_ID or TARGET_OBJ or SOURCE_OBJ is not defined
	exit 1
fi

OBJCOPY=${EMBOX_CROSS_COMPILE}objcopy
OBJDUMP=${EMBOX_CROSS_COMPILE}objdump
if [ "$COMPILER" = "clang" ]; then
	CC=clang
else
	CC=${EMBOX_CROSS_COMPILE}gcc
fi
LD=${EMBOX_CROSS_COMPILE}ld

CMD_WRAPPER_SRC=$ROOT_DIR/mk/script/application_template.c

redefd_main_obj=$SOURCE_OBJ.redef.o
cmd_wrapper_obj=$SOURCE_OBJ.cmd.o
localize_symbols=$SOURCE_OBJ.lsyms.txt
main_rename_name=main_$MODULE_ID

# XXX hack to expand -D"__impl_x(...)=..." came from CPPFLAGS. These flags are
# from # EXTERNAL_MAKE_FLAGS and seems to have unnecessary escaping.
# CPPFLAGS annoys only, in string expansion impl_x definition turns out
# to be '-D"__impl_x(...)=..."' which isn't considered by gcc as macro
# definition ('-D__impl_x(...)=...' isn't as well).
# Next line expands '-D"__impl_x(...)=..."' to -D"__impl_x(...)=..."
#EMBOX_CPPFLAGS=$(eval echo $EMBOX_CPPFLAGS)

if [ ${MAIN_STRIPPING_LOCALS:-no} = yes ]; then
	$OBJDUMP -t $SOURCE_OBJ | grep " g " | tr -s ' ' | cut -d \  -f 5 | \
		(grep -v main || true) > $localize_symbols

	echo >> $localize_symbols #objcopy dislikes empty files
	$OBJCOPY --localize-symbols=$localize_symbols \
		--redefine-sym=main=$main_rename_name \
		$SOURCE_OBJ $redefd_main_obj
else
	$OBJCOPY --redefine-sym=main=$main_rename_name \
		$SOURCE_OBJ $redefd_main_obj
fi

$CC -DMAIN_ROUTING_NAME=$main_rename_name \
	-D__EMBUILD_MOD__=$MODULE_ID \
	-DCXX_CMD=$([ "${CXX_CMD:-no}" = "yes" ] && echo 1 || echo 0) \
	$EMBOX_CFLAGS \
	$EMBOX_CPPFLAGS \
	-c -o $cmd_wrapper_obj $CMD_WRAPPER_SRC

$LD -r $EMBOX_LDFLAGS $redefd_main_obj $cmd_wrapper_obj -o $TARGET_OBJ

