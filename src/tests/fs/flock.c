/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 * @author Anton Kozlov
 */

static struct thread *fftt, *sftt;
static void *first_flock_test_thread(void *arg);
static void *second_flock_test_thread(void *arg);

static void fs_test_flock(void) {
	int fd;
	int l = 200, h = 210;

	/* Prepare file and threads for test */
	test_assert(-1 != (fd = open(FS_FLOCK, O_CREAT, S_IRUSR | S_IWUSR)));

	fftt = thread_create(THREAD_FLAG_SUSPENDED, first_flock_test_thread,
			(void *) &fd);
	test_assert_zero(ptr2err(fftt));


	sftt = thread_create(THREAD_FLAG_SUSPENDED, second_flock_test_thread,
			(void *) &fd);
	test_assert_zero(ptr2err(sftt));

	test_assert_zero(schedee_priority_set(&fftt->schedee, l));
	test_assert_zero(schedee_priority_set(&sftt->schedee, l));

	test_assert_zero(thread_launch(fftt));
	test_assert_zero(thread_join(fftt, NULL));
	test_assert_zero(thread_join(sftt, NULL));

	test_assert_emitted("abcdefg");

	/* Test cleanup */
	test_assert_zero(remove(FS_FLOCK));
}

static void *first_flock_test_thread(void *arg) {
	int fd = *((int *) arg);

	test_emit('a');
	test_assert_zero(flock(fd, LOCK_EX));
	test_emit('b');
	test_assert_zero(thread_launch(sftt));
	test_emit('d');
	test_assert_zero(flock(fd, LOCK_UN));
	test_emit('g');

	return NULL;
}

static void *second_flock_test_thread(void *arg) {
	int fd = *((int *) arg);

	test_emit('c');

	/* Try to non-blocking acquire busy lock */
	test_assert(-1 == flock(fd, LOCK_EX | LOCK_NB));
	test_assert(EWOULDBLOCK == errno);

	/* Acquire just unlocked by fftt exclusive lock */
	test_assert_zero(flock(fd, LOCK_EX));
	test_emit('e');

	/* Convert lock to shared */
	test_assert_zero(flock(fd, LOCK_SH));

	/* Acquire one more shared lock */
	test_assert_zero(flock(fd, LOCK_SH | LOCK_NB));

	/* Release first shared lock */
	test_assert_zero(flock(fd, LOCK_UN));

	/* Convert share to exclusive */
	test_assert_zero(flock(fd, LOCK_EX));

	/* Release second lock */
	test_assert_zero(flock(fd, LOCK_UN | LOCK_NB));

	test_emit('f');

	return NULL;
}
