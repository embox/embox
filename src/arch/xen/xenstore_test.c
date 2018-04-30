/**
 * @file
 * @brief
 *
 * @author
 * @date
 */

#include <string.h>
#include <xenstore.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("xenstore smoke tests");

TEST_CASE("ls should list own domain entries") {
	char domid[32];
	int domid_len = xenstore_read("domid", domid, sizeof(domid));
	test_assert(0 < domid_len);
	test_assert(domid[0] != 'E'); // not E*** error id
	domid[domid_len] = '\0';

	char local_dom[32] = "/local/domain/";
	strcat(local_dom, domid);
	char ls[128];
	int len = xenstore_ls(local_dom, ls, sizeof(ls));
	test_assert(0 < len);
	test_assert(ls[0] != 'E');
}
