//tst_srv.c

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>
#include <sys/types.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include <kernel/time/ktime.h>
#include <net/inetdevice.h>
#include <embox/cmd.h>
#include <kernel/thread.h>

#define MAX_ITER_COUNT 	LLONG_MAX

EMBOX_CMD(tst_srv);

static void pi_test(int accuracy) {
	unsigned long long n=0;
	double s1=0, s2=0;
	double acc_delta=0;

	acc_delta=1/pow(10,accuracy);

	while (n<MAX_ITER_COUNT) {
		n++;
		s1=s2+(4.0/(2.0*n-1.0)); /* (S1 - upper estimate ) */
		n++;
		s2=s1-4.0/(2.0*n-1.0); /* (S2 - lover estimate ) */
		if (s1-s2 < acc_delta ) break;
		}
}




static void * client_process(void * args) {

	char buf[256];
	struct timeval tv,tv1,tv2,tv3;

	fd_set fds;
	int bytes;

	int sock;

	sock=(int)args;
	FD_ZERO(&fds);
	FD_SET(sock,&fds);
	printf("Wait request...\n");

	usleep(10);
	bytes=0;
	memset(buf,0,sizeof(buf));
	bytes = recv( sock, buf, sizeof(buf)-1, 0);
	if(bytes==0){
		usleep(50);
		bytes = recv( sock, buf, sizeof(buf)-1, 0);
		memset(buf,0,sizeof(buf));
		sprintf(buf,"$$$");
		send(sock,buf,strlen(buf)+1,0);
	}
	if(bytes>0) {
		if((!strncmp(buf,"test",4)) && (strlen(buf) == 4)){
			printf("Correct request\n");
			ktime_get_timeval(&tv);
		printf(".");
			pi_test(7);
		printf(".");
			ktime_get_timeval(&tv1);
		printf(".");
			tv2.tv_sec=tv1.tv_sec-tv.tv_sec;
			if(tv1.tv_usec>=tv.tv_usec){
				tv2.tv_usec=tv1.tv_usec-tv.tv_usec;
			}
			else{
				tv2.tv_usec=tv1.tv_usec+1000000l-tv.tv_usec;
				tv2.tv_sec--;
			}
		printf(".");
			ktime_get_timeval(&tv);
			usleep(1500000l);
			ktime_get_timeval(&tv1);
		printf(".");
			tv3.tv_sec=tv1.tv_sec-tv.tv_sec;
			if(tv1.tv_usec>=tv.tv_usec){
				tv3.tv_usec=tv1.tv_usec-tv.tv_usec;
			}
			else{
				tv3.tv_usec=tv1.tv_usec+1000000l-tv.tv_usec;
				tv3.tv_sec--;
			}
		printf(".");
			memset(buf,0,sizeof(buf));
			sprintf(buf,"\t1: %d.%06d c.\n\t2: %d.%06d c\n",(int)tv2.tv_sec,(int)tv2.tv_usec,(int)tv3.tv_sec,(int)tv3.tv_usec);
		printf(".");
			printf("send %s\n",buf);
			send(sock,buf,strlen(buf)+1,0);
		}
		else{
			printf("Incorrect request\n");
		}
	}
	else{
			printf("len=0\n");
	}

	close(sock);
	return (void*)0;
}

static int tst_srv(int argc, char **argv){
	int res, host;
	socklen_t addr_len;
	struct sockaddr_in addr;
	struct thread *tr;
	addr.sin_family = AF_INET;
	addr.sin_port= htons(7770);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	pi_test(6);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (host < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		return host;
	}

	res = bind(host, (struct sockaddr *)&addr, sizeof(addr));
	if (res < 0) {
		printf("Error.. bind() failed. errno=%d\n", errno);
		return res;
	}

	res = listen(host, 10);
	if (res < 0) {
		printf("Error.. listen() failed. errno=%d\n", errno);
		return res;
	}

	while (1) {
		res = accept(host,(struct sockaddr *)&addr, &addr_len);
		if (res <= 0) {
			/* error code in client, now */
			printf("Error.. accept() failed. errno=%d\n", errno);
			continue;
		}
		if(thread_create(&tr, 0, client_process, (void *) res)){
			printf("Error.. thread_create() failed. errno=%d\n", errno);
			continue;
		}
		thread_set_priority(tr, 160);
		thread_detach(tr);
	}
	close(host);
    return 0;
}
