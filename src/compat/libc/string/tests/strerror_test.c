#include <string.h>
#include <embox/test.h>
#include <errno.h>

EMBOX_TEST_SUITE("string/strerror test");

TEST_CASE("text of ENOERR is No error"){
	/* Source number */
	int errnum = ENOERR;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"No error");
}

TEST_CASE("text of EPERM is Not permitted"){
	/* Source number */
	int errnum = EPERM;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"Not permitted");
}

TEST_CASE("text of ENOENT is No such entity"){
	/* Source number */
	int errnum = ENOENT;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"No such entity");
}

TEST_CASE("text of ESRCH is No such process"){
	/* Source number */
	int errnum = ESRCH;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"No such process");
}


TEST_CASE("text of EINTR is Operation interrupted"){
	/* Source number */
	int errnum = EINTR;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"Operation interrupted");
}


TEST_CASE("text of EIO is I/O error"){
	/* Source number */
	int errnum = EIO;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"I/O error");
}


TEST_CASE("text of EBADF is Bad file handle"){
	/* Source number */
	int errnum = EBADF;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"Bad file handle");
}


TEST_CASE("text of EAGAIN is Try again later"){
	/* Source number */
	int errnum = EAGAIN;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"Try again later");
}


TEST_CASE("text of ENOMEM is Out of memory"){
	/* Source number */
	int errnum = ENOMEM;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"Out of memory");
}


TEST_CASE("text of EBUSY is Resource busy"){
	/* Source number */
	int errnum = EBUSY;

	/* Checking if the error text matches */
	test_assert_equal(strerror(errnum),"Resource busy");
}
