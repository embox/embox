
$_SUBDIRS := $(filter-out _template,$($_SUBDIRS))

$_PACKAGE := embox.test

$_CPPFLAGS += -I$($_SELFDIR)/_misc/include

# Util for adding new test.
# Usage example: TEST_NAME=test make at

_TEST_TEMPLATE = _template

add_test at:
	@[[ -d $(TEST_NAME) ]] || mkdir -p $(TEST_NAME)
	cat $(_TEST_TEMPLATE)/Makefile > $(TEST_NAME)/Makefile
	cat $(_TEST_TEMPLATE)/test.c > $(TEST_NAME)/$(TEST_NAME).c
	svn add $(TEST_NAME)
