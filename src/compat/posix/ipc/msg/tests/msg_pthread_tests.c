/**
 * @file
 *
 * @date Jul 20, 2023
 * @author Anton Bondarev
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("test tests for <sys/msg.h>");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

#define MAXBUFSIZE     12

struct msgbuf {
	long mtype;
	char mtext [MAXBUFSIZE];
};

static int msqid = -1;
static key_t key = IPC_PRIVATE;

struct thread_args {
	int msqid;
	struct msgbuf *tmp_buf;
};

static void *start_routine1(void *arg) {
	int rtrn;
	int msgflg = IPC_NOWAIT;
	int msgsz;
	struct msgbuf tx_buf[1];
	int i;
	struct thread_args *thread_args = arg;
	struct msgbuf *tmp_buf = thread_args->tmp_buf;
	int msqid = thread_args->msqid;

	msgsz = MAXBUFSIZE;

	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));

	return NULL;
}

static void *start_routine2(void *arg) {
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf tx_buf[2];
	int i;
	struct thread_args *thread_args = arg;
	struct msgbuf *tmp_buf = &thread_args->tmp_buf[1];
	int msqid = thread_args->msqid;

	msgsz = MAXBUFSIZE;
	/* sending */
	/* first msg */
	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i + 0x10;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));
	/* second msg */
	for (i = 0; i < sizeof(tx_buf[1].mtext); i++) {
		tx_buf[1].mtext[i] = (char)i + 0x20;
	}
	rtrn = msgsnd(msqid, &tx_buf[1], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[1], &tx_buf[1], sizeof(struct msgbuf));
	memset(&tx_buf[1], 0, sizeof(struct msgbuf));

	return NULL;
}

TEST_CASE("send from a new pthread rcv form a main thread ") {
	pthread_t thread1;
	int rtrn;
	int msgflg = IPC_NOWAIT;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[1];
	struct thread_args thread_args = { msqid, tmp_buf };


	pthread_create(&thread1, NULL, start_routine1, &thread_args);
	pthread_join(thread1, NULL);

	msgsz = MAXBUFSIZE;

	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);
}

TEST_CASE("send from 2 diff pthreads rcv from a main thread") {
	pthread_t thread1;
	pthread_t thread2;
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[3];
	struct thread_args thread_args = { msqid, &tmp_buf[0] };

	pthread_create(&thread1, NULL, start_routine1, &thread_args);
	pthread_join(thread1, NULL);

	pthread_create(&thread2, NULL, start_routine2, &thread_args);
	pthread_detach(thread2);

	msgsz = MAXBUFSIZE;

	memset(rx_buf, 0, sizeof(rx_buf));

	/* reading */
	/* msg from first thread  */
	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);

	memset(rx_buf, 0, sizeof(rx_buf));
	/* first msg from second thread */
	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[1].mtext, rx_buf[0].mtext, msgsz);

	memset(rx_buf, 0, sizeof(rx_buf));
	/* second msg from second thread */
	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[2].mtext, rx_buf[0].mtext, msgsz);
}

static int setup_suite(void) {
	int msgflg;

	msgflg = IPC_CREAT;
	msqid = msgget (key, msgflg);
	if (msqid == -1) {
		return -1;
	}

	return 0;
}

static int teardown_suite(void) {
	int rtrn;

	rtrn = msgctl(msqid, IPC_RMID, NULL);
	if (rtrn == -1) {
		return -1;
	}

	return 0;
}
