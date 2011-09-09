$_PACKAGE := embox.cmd

$_CPPFLAGS += -I$(SRC_DIR)/include/posix

$_MODS += lisp500
$_SRCS-lisp500 += lisp500.c
$_DEPS-lisp500 += embox.fs.core

$(SELFDIR)/lisp500.c:$(BUILD_DIR)/rootfs/init500.lisp


$(BUILD_DIR)/rootfs/init500.lisp:$(SELFDIR)/init500.lisp
	@mkdir $(BUILD_DIR)/rootfs
	cp $< $@
