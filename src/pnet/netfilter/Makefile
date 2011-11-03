$_PACKAGE := embox.pnet.netfilter

$_MODS += match
$_SRCS-match += match.c rule.c
$_DEPS-match += embox.pnet.core

$_MODS += rules_init
$_SRCS-rules_init += rules_init.c
$_DEPS-rules_init += match

$_CPPFLAGS += -I$(SRC_DIR)/include/posix
$_CPPFLAGS-rules_init += -I$(CONF_DIR)



