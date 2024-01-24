/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 18.07.23
 */
#include <stddef.h>

#include <embox/test.h>
#include <lib/libds/msg_buff.h>

EMBOX_TEST_SUITE("tests for msg_buff");

#define MAX_MSG_SIZE  16
#define MSG_BUFF_SIZE (32 + (4 * sizeof(size_t)))

TEST_CASE("Write/read messages") {
	char storage[MSG_BUFF_SIZE];
	char message[MAX_MSG_SIZE + 1];
	struct msg_buff buf;
	size_t msg_len;
	size_t space;

	msg_buff_init(&buf, storage, sizeof(storage));

	test_assert_true(msg_buff_empty(&buf));

	msg_len = msg_buff_dequeue(&buf, message, MSG_BUFF_SIZE);

	test_assert_equal(msg_len, 0);

	msg_len = msg_buff_enqueue(&buf, "abcd", 4);
	test_assert_equal(msg_len, 4);

	msg_len = msg_buff_enqueue(&buf, "ef", 2);
	test_assert_equal(msg_len, 2);

	msg_len = msg_buff_enqueue(&buf, "hello", 5);
	test_assert_equal(msg_len, 5);

	msg_len = msg_buff_enqueue(&buf, "--------------------------", 26);
	test_assert_equal(msg_len, 0);

	space = msg_buff_space(&buf);
	test_assert_equal(space, 20);

	msg_len = msg_buff_enqueue(&buf, "---------------!----", 20);
	test_assert_equal(msg_len, 20);

	msg_len = msg_buff_dequeue(&buf, message, MAX_MSG_SIZE);
	test_assert_equal(msg_len, 4);
	message[msg_len] = '\0';
	test_assert_str_equal("abcd", message);

	msg_len = msg_buff_dequeue(&buf, message, MAX_MSG_SIZE);
	test_assert_equal(msg_len, 2);
	message[msg_len] = '\0';
	test_assert_str_equal("ef", message);

	msg_len = msg_buff_dequeue(&buf, message, MAX_MSG_SIZE);
	test_assert_equal(msg_len, 5);
	message[msg_len] = '\0';
	test_assert_str_equal("hello", message);

	msg_len = msg_buff_dequeue(&buf, message, MAX_MSG_SIZE);
	test_assert_equal(msg_len, 0);
	message[MAX_MSG_SIZE] = '\0';
	test_assert_str_equal("---------------!", message);
}
