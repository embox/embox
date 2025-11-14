#!/usr/bin/env bash

cat << 'EOF'
#!/usr/bin/env bash

# Auto-generated file. Do not edit.

case " $@ " in
	*" -shared "* | \
	*" -pthread "* | \
	*" -rdynamic "* | \
	*" -l"* | \
	*" -I/usr/"*)
		$0 $(echo " $@" | sed \
		-e 's/ -shared//g' \
		-e 's/ -pthread//g' \
		-e 's/ -rdynamic//g' \
		-e 's/ -l[0-9a-zA-Z_]*//g' \
		-e 's/ -I\/usr\/[0-9a-zA-Z_]*//g'); \
		exit $?;;
esac

EOF

if [[ "$DIST_GEN" == "y" ]]; then
cat << 'EOF'
EMBOX_DIST_DIR=$(realpath $(dirname $(realpath $0))/../.)
EMBOX_DIST_INC_DIR=$EMBOX_DIST_DIR/include
EMBOX_DIST_LIB_DIR=$EMBOX_DIST_DIR/lib

EOF
fi

cat << EOF
EMBOX_IMPORTED_CPPFLAGS="$EMBOX_IMPORTED_CPPFLAGS"
EMBOX_IMPORTED_CFLAGS="$EMBOX_IMPORTED_CFLAGS"
EMBOX_IMPORTED_M_CFLAGS="$EMBOX_IMPORTED_M_CFLAGS"
EMBOX_IMPORTED_CXXFLAGS="$EMBOX_IMPORTED_CXXFLAGS"
EMBOX_IMPORTED_LDFLAGS="$EMBOX_IMPORTED_LDFLAGS"
EMBOX_IMPORTED_LDFLAGS_FULL="$EMBOX_IMPORTED_LDFLAGS_FULL"

EOF

cat << 'EOF'
cppflags="$EMBOX_DEPS_CPPFLAGS_BEFORE $EMBOX_IMPORTED_CPPFLAGS $EMBOX_DEPS_CPPFLAGS_AFTER"

cmd=$(basename $0)
cmd=${cmd#embox-}

case $cmd in
	gcc|clang)   arg_line_before="$EMBOX_IMPORTED_CFLAGS";;
	g++|clang++) arg_line_before="$EMBOX_IMPORTED_CXXFLAGS";;
esac

arg_line_after="$cppflags $EMBOX_IMPORTED_LDFLAGS -Wl,-r"

EOF

if [[ "$COMPILER" == "gcc" ]]; then
cat << 'EOF'
if [[ "$EMBOX_GCC_LINK" == "full" ]]; then
arg_line_after="$cppflags $EMBOX_IMPORTED_LDFLAGS $EMBOX_IMPORTED_LDFLAGS_FULL"
fi

EOF
fi

cat << 'EOF'
case " $@ " in
	*" -c "*) arg_line_after="$cppflags";;
	*" -E "*) arg_line_after="$cppflags"; arg_line_before="$EMBOX_IMPORTED_M_CFLAGS";;
esac

EOF

# Command line arguments ("$@") follow imported cflags and cxxflags and can override them.
# Imported cppflags follow command line arguments ("$@") for '#include_next' to work.
# Imported ldflags must follow input files from command line arguments ("$@").
if [[ "$COMPILER" == "gcc" ]]; then
cat << 'EOF'
${EMBOX_CROSS_COMPILE}$cmd $arg_line_before "$@" $arg_line_after
exit $?

EOF
else
cat << 'EOF'
$cmd $arg_line_before "$@" $arg_line_after
exit $?

EOF
fi
