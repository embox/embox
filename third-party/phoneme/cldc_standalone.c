/**
 * @file
 * @brief phoneME standalone Monty VM glue layer
 *
 * @date 24.12.12
 * @author Felix Sulima
 */

#include <javacall_time.h>
#include <pcsl_memory.h>
#include <pcsl_print.h>
#include <kernel/task.h>
#include <jvm.h>
#include <jvmspi.h>
#include <sni.h>

#include <stdio.h>
#include <assert.h>

#include "cldc_standalone.h"

static void *phoneme_run(void *data);

struct __jvm_params {
	int argc;
	char **argv;
	int code;
};

int phoneme_cldc(int argc, char **argv) {
	struct __jvm_params params = {
			.argc = argc,
			.argv = argv,
			.code = -1
	};

	new_task(phoneme_run, &params);
	while(!list_empty(&task_self()->children)) { } /* XXX make it throw signals */

	return params.code;
}

/* In the most part implementation is copied from Main_javacall.cpp */
static void *phoneme_run(void *data) {
	int argc;
	char **argv;
	struct __jvm_params *params = (struct __jvm_params *)data;

	argc = params->argc;
	argv = params->argv;

	pcsl_mem_initialize(NULL, -1);
	JVM_Initialize();

	argc --;
	argv ++;

	while (true) {
	    int n = JVM_ParseOneArg(argc, argv);
	    if (n < 0) {
	    	printf("Unknown argument: %s\n", argv[0]);
	    	JVMSPI_DisplayUsage(NULL);
	    	params->code = -1;
	    	goto end;
	    } else if (n == 0) {
	    	break;
	    }
	    argc -= n;
	    argv += n;
	}

	  if (JVM_GetConfig(JVM_CONFIG_SLAVE_MODE) == KNI_FALSE) {
	    // Run the VM in regular mode -- JVM_Start won't return until
	    // the VM completes execution.
	    params->code = JVM_Start(NULL, NULL, argc, argv);
	  } else {
	    // Run the VM in slave mode -- we keep calling JVM_TimeSlice(),
	    // which executes bytecodes for a small amount and returns. This
	    // mode is necessary for platforms that need to keep the main
	    // control loop outside of of the VM.

	    JVM_Start(NULL, NULL, argc, argv);

	    for (;;) {
	      jlong timeout = JVM_TimeSlice();
	      if (timeout <= -2) {
	        break;
	      } else {
	        int blocked_threads_count;
	        JVMSPI_BlockedThreadInfo * blocked_threads;

	        blocked_threads = SNI_GetBlockedThreads(&blocked_threads_count);
	        JVMSPI_CheckEvents(blocked_threads, blocked_threads_count, timeout);
	      }
	    }

	    params->code = JVM_CleanUp();
	  }

end:
    pcsl_mem_finalize();

    return NULL;
}

void JVMSPI_PrintRaw(const char* s, int length) {
	/* Print the string to the standard output device */
	pcsl_print_chars(s, length);
}

void JVMSPI_Exit(int code) {
	pcsl_mem_finalize();
	task_exit(NULL);
	/* Terminate the current process */
	// ToDo: terminate task
	return;
}

void embox_phoneme_exit(int status) {
	/* Terminate the current process */
	// ToDo: terminate task
	return;
}

/* Copied from PCSLSocket.cpp */
void JVMSPI_CheckEvents(JVMSPI_BlockedThreadInfo *blocked_threads,
                        int blocked_threads_count,
                        jlong timeout) {
  assert(blocked_threads_count == 0,
            "Vanilla VM never calls SNI_BlockThread");
  if (timeout > 0) {
    javacall_time_sleep(timeout);
  }
}
