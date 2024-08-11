#include <kernel/thread.h>
#include <kernel/cpu/cpu.h>
#include <kernel/spinlock.h>
#include <kernel/thread/thread_alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile int shared_counter = 0;
static spinlock_t lock = SPIN_STATIC_UNLOCKED;

static void *thread_func(void *arg) {
    int iterations = 10;
    for (int i = 0; i < iterations; i++) {
        spin_lock(&lock);
        int local_counter = ++shared_counter;
        spin_unlock(&lock);

        printf("CPU %d, iteration %d, shared_counter: %d\n", cpu_get_id(), i, local_counter);
    }
    return NULL;
}

void start_thread_on_cpu(int cpu_id) {
    struct thread *t = thread_create(THREAD_FLAG_SUSPENDED, thread_func, NULL);
    if (!t) {
        printf("Failed to create thread on CPU %d\n", cpu_id);
        return;
    }

    cpu_bind(cpu_id, t);
    thread_launch(t);
}

int count, all_chars;
char b = '\n';
void * thd_1(void *arg){
	for(int i = 0;count < all_chars; i++){
		if((i % 10000000) == 0){
			count++;
			if(count % 70 == 0) write(2,&b,1);

			write(2,(char *)arg,1);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	struct thread * tid1, *tid2, *tid3, *tid4, *tid5;
	char string[]="0123456789";

	if(argc > 1){
		all_chars = atoi(argv[1]);
		count = 0;
	} else{
		all_chars = 200;
		count = 0;
	}

	sched_lock();
	tid1 = thread_create(0,thd_1,string+1);
	tid2 = thread_create(0,thd_1,string+2);
	tid3 = thread_create(0,thd_1,string+3);
	tid4 = thread_create(0,thd_1,string+4);
	tid5 = thread_create(0,thd_1,string+5);

	sched_affinity_set(&tid1->schedee.affinity,1);
	sched_affinity_set(&tid2->schedee.affinity,1);
	sched_affinity_set(&tid3->schedee.affinity,1);
	sched_affinity_set(&tid4->schedee.affinity,2);
	sched_affinity_set(&tid5->schedee.affinity,2);

	sched_unlock();

	thread_join(tid1,NULL);
	thread_join(tid2,NULL);
	thread_join(tid3,NULL);
	thread_join(tid4,NULL);
	thread_join(tid5,NULL);
}
