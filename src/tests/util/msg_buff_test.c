/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 18.07.23
 */
#include <stddef.h>

#include <embox/test.h>
#include <util/msg_buff.h>

EMBOX_TEST_SUITE("util/ring_buff test");

TEST_CASE("Write/read messages") {
	char msg_buff_storage[32];
	char message[16];
	struct msg_buff buf;
	size_t msg_len;

	msg_buff_init(&buf, msg_buff_storage, sizeof(msg_buff_storage));

	msg_buff_enqueue(&buf, "abcd", 4);
	msg_buff_enqueue(&buf, "ef", 2);
	msg_buff_enqueue(&buf, "hello", 5);
	msg_buff_enqueue(&buf, "--------------------------", 26);

	msg_len = msg_buff_dequeue(&buf, message, sizeof(message));
	message[msg_len] = '\0';
	test_assert_str_equal("abcd", message);

	msg_len = msg_buff_dequeue(&buf, message, sizeof(message));
	message[msg_len] = '\0';
	test_assert_str_equal("ef", message);

	msg_len = msg_buff_dequeue(&buf, message, sizeof(message));
	message[msg_len] = '\0';
	test_assert_str_equal("hello", message);

	msg_len = msg_buff_dequeue(&buf, message, sizeof(message));
	message[msg_len] = '\0';
	test_assert_str_equal("", message);
}
