#!/usr/bin/env bash

cat << 'EOF'
#!/usr/bin/env bash

# Auto-generated file. Do not edit.

cmd=$(basename $0)
cmd=${cmd#embox-}

${EMBOX_CROSS_COMPILE}${cmd} "$@"
exit $?

EOF
