/**
 * @file
 *
 * @date Jul 20, 2023
 * @author Anton Bondarev
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <kernel/lthread/lthread.h>

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

struct lthread_args {
	int msqid;
	struct msgbuf *tmp_buf;
};

struct msg_lthread_arg {
	struct lthread lthread;
	struct lthread_args lthread_args;
};

#define LTHREAD_ARG(lt) \
	&((struct msg_lthread_arg *)(void *)lt)->lthread_args

static int lthread_routine1(struct lthread *self) {
	int rtrn;
	int msgflg = IPC_NOWAIT;
	int msgsz;
	struct msgbuf tx_buf[1];
	int i;
	struct lthread_args *thread_args = LTHREAD_ARG(self);
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

	return 0;
}

static int lthread_routine2(struct lthread *self) {
	int rtrn;
	int msgflg = IPC_NOWAIT;
	int msgsz;
	struct msgbuf tx_buf[2];
	int i;
	struct lthread_args *thread_args = LTHREAD_ARG(self);
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

	return 0;
}

TEST_CASE("send from a new pthread rcv form a main thread ") {
	struct msg_lthread_arg lthread1;
	struct msg_lthread_arg lthread2;
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[3];

	lthread1.lthread_args.msqid = msqid;
	lthread1.lthread_args.tmp_buf = tmp_buf;

	lthread_init(&lthread1.lthread, lthread_routine1);
	lthread_launch(&lthread1.lthread);
	lthread_join(&lthread1.lthread); /* we want to make sure messages sequence */

	lthread2.lthread_args.msqid = msqid;
	lthread2.lthread_args.tmp_buf = tmp_buf;

	lthread_init(&lthread2.lthread, lthread_routine2);
	lthread_launch(&lthread2.lthread);

	msgsz = MAXBUFSIZE;

	memset(rx_buf, 0, sizeof(rx_buf));
	/* msg from first thread */
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
