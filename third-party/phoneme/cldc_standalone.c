/**
 * @file
 * @brief phoneME standalone Monty VM glue layer
 *
 * @date 24.12.12
 * @author Felix Sulima
 */

typedef long long __int64;
#include <pcsl_memory.h>
#include <jvm.h>
#include <jvmspi.h>

#include <stdio.h>

#include "cldc_standalone.h"

/* Implementation copied from Main_javacall.cpp */
int phoneme_cldc(int argc, char **argv) {
	int code;

	pcsl_mem_initialize(NULL, -1);
	JVM_Initialize();

	argc --;
	argv ++;

	while (true) {
	    int n = JVM_ParseOneArg(argc, argv);
	    if (n < 0) {
	    	printf("Unknown argument: %s\n", argv[0]);
	    	JVMSPI_DisplayUsage(NULL);
	    	code = -1;
	    	goto end;
	    } else if (n == 0) {
	    	break;
	    }
	    argc -= n;
	    argv += n;
	}

    code = JVM_Start(NULL, NULL, argc, argv);

end:
    pcsl_mem_finalize();

    return code;
}
