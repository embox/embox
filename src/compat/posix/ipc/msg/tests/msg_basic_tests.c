/**
 * @file
 *
 * @date Jul 20, 2023
 * @author Anton Bondarev
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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

TEST_CASE("send/rcv") {
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[1];
	struct msgbuf tx_buf[1];
	int i;

	msgsz = MAXBUFSIZE;
	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));

	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);
}

TEST_CASE("rcv with IPC_NOWAIT flas") {
	int rtrn;
	int msgflg = IPC_NOWAIT;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[1];
	struct msgbuf tx_buf[1];
	int i;

	msgsz = MAXBUFSIZE;

	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn == -1); /* errno == ENOMSG */


	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));

	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);
}


TEST_CASE("send/rcv") {
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[1];
	struct msgbuf tx_buf[1];
	int i;

	msgsz = MAXBUFSIZE;
	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));

	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);
}

TEST_CASE("send/rcv 2 messages") {
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf rx_buf[2];
	struct msgbuf tmp_buf[2];
	struct msgbuf tx_buf[2];
	int i;

	msgsz = MAXBUFSIZE;
	/* sending */
	/* first msg */
	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));
	/* second msg */
	for (i = 0; i < sizeof(tx_buf[1].mtext); i++) {
		tx_buf[1].mtext[i] = (char)i + 0x10;
	}
	rtrn = msgsnd(msqid, &tx_buf[1], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[1], &tx_buf[1], sizeof(struct msgbuf));
	memset(&tx_buf[1], 0, sizeof(struct msgbuf));

	/* reading */
	/* first msg */
	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);
	/* second msg */
	rtrn = msgrcv(msqid, &rx_buf[1], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[1].mtext, rx_buf[1].mtext, msgsz);
}

TEST_CASE(" get state with msgctl") {
	int rtrn;
	int msgflg = 0;
	int msgsz;
	struct msgbuf rx_buf[1];
	struct msgbuf tmp_buf[1];
	struct msgbuf tx_buf[1];
	int i;
	struct msqid_ds msqid_ds;

	msgsz = MAXBUFSIZE;

	rtrn = msgctl(msqid, IPC_STAT, &msqid_ds);
	test_assert(rtrn != -1);
	test_assert(msqid_ds.msg_qnum == 0);

	for (i = 0; i < sizeof(tx_buf[0].mtext); i++) {
		tx_buf[0].mtext[i] = (char)i;
	}
	rtrn = msgsnd(msqid, &tx_buf[0], msgsz, msgflg);
	test_assert(rtrn != -1);

	memcpy(&tmp_buf[0], &tx_buf[0], sizeof(struct msgbuf));
	memset(&tx_buf[0], 0, sizeof(struct msgbuf));

	rtrn = msgctl(msqid, IPC_STAT, &msqid_ds);
	test_assert(rtrn != -1);
	test_assert(msqid_ds.msg_qnum == 1);

	rtrn = msgrcv(msqid, &rx_buf[0], msgsz, 0, msgflg);
	test_assert(rtrn != -1);
	test_assert_mem_equal(tmp_buf[0].mtext, rx_buf[0].mtext, msgsz);

	rtrn = msgctl(msqid, IPC_STAT, &msqid_ds);
	test_assert(rtrn != -1);
	test_assert(msqid_ds.msg_qnum == 0);
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
