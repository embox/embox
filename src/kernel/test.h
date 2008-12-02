#ifndef _TEST_H_
#define _TEST_H_

// child process started
extern volatile BOOL chproc_started;
// child process return value
extern volatile BOOL chproc_ret_val;
// accept or not process aborting
extern volatile BOOL chproc_abort_accept;


//extern volatile BOOL show_test_messages;

//void test_allow_aborting();
//void test_disable_abortrting ();
//void test_enable_timing();
//void test_timer_handler();
void test_run(WORD pfunc);



#endif //_TEST_H_
