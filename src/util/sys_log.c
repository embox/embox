/**
 * @file
 * @brief A temporary substitube for STDERR
 *
 * @date 13.03.12
 * @author Timur Abdukadyrov
 *
 */

#include <stdio.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <util/macro.h>

#include <util/sys_log.h>

#include <kernel/printk.h>

#include <framework/mod/options.h>

#define N_DEBUG_MSG OPTION_GET(NUMBER,msg_n)  /* Quantity of messages in a queue */
#define MAX_MSG_LENGTH (OPTION_GET(NUMBER,msg_max_len)+1)  /* Length of a message + '\0' */
#define MAX_MODULE_NAME_LENGTH (OPTION_GET(NUMBER,modname_max_len)+1)  /* module name */
#define MAX_FUNC_NAME_LENGTH (OPTION_GET(NUMBER,funcname_max_len)+1)  /* function name */

static const char *msg_trunc_msg  = "The message is to long. Trunctuated to 50 symbols\n";
static const char *mod_trunc_msg  = "The module name is to long. Trunctuated to 20 symbols\n";
static const char *func_trunc_msg = "The function name is to long. Trunctuated to 20 symbols\n";

static bool annoy = false;
static bool annoy_types[] = {true, true, true, true};

/* message structure in a queue */
typedef struct debug_msg{
	char msg[MAX_MSG_LENGTH];
	char module[MAX_MODULE_NAME_LENGTH];
	char func[MAX_FUNC_NAME_LENGTH];
	unsigned int serial;
	int msg_type;
} debug_msg_t;

/* POOL_DEF(debug_msg_pool, struct debug_msg, N_DEBUG_MSG); */
/* static char pool[N_MSG*N_DEBUG_MSG]; */
static debug_msg_t log[N_DEBUG_MSG];

static unsigned int N_message = 0;
static volatile unsigned int serial = 0;
/* static unsigned int tail = 0; */

static char* types_str[] = {"INFO", "WARNING", "ERROR", "DEBUG"};

bool syslog_toggle_intrusive(bool *types){
	int i;

	/* set types to display in intrusive mode */
	if(types != NULL)
		for(i=0; i<LT_MAX-1; i++)
			annoy_types[i] = types[i];

	return annoy = annoy ? false : true;
}

void system_log(const char *msg, char *module, char *func, int msg_type){

	/* truncuate if necessary */
	if(strlen(msg)>MAX_MSG_LENGTH){
		if(strlen(msg_trunc_msg)<=MAX_MSG_LENGTH)/* just in case*/
			/* debug_printf(msg_trunc_msg, MODULE_NAME, "debug_printf"); */
			LOG_WARN("system_log", msg_trunc_msg);
		strncpy(log[N_message].msg, msg, MAX_MSG_LENGTH);
	}else
		strcpy(log[N_message].msg, msg);
	if(strlen(module)>MAX_MODULE_NAME_LENGTH){
		if(strlen(mod_trunc_msg)<=MAX_MSG_LENGTH)/* just in case*/
			/* debug_printf(mod_trunc_msg, MODULE_NAME, "debug_printf"); */
			LOG_WARN("system_log", mod_trunc_msg);
		strncpy(log[N_message].module, module, MAX_MODULE_NAME_LENGTH);
	}else
		strcpy(log[N_message].module, module);
	if(strlen(func)>MAX_FUNC_NAME_LENGTH){
		if(strlen(func_trunc_msg)<=MAX_MSG_LENGTH)/* just in case*/
			/* debug_printf(func_trunc_msg, MODULE_NAME, "debug_printf"); */
			LOG_WARN("system_log", func_trunc_msg);
		strncpy(log[N_message].func, func, MAX_FUNC_NAME_LENGTH);
	}else
		strcpy(log[N_message].func, func);
	log[N_message].serial = serial;
	log[N_message].msg_type = msg_type;

	/* cyclic incrementation */
	serial++;
	N_message++;
	if(N_message == N_DEBUG_MSG)
		N_message = 0;

	/* if user asked to print all log messages to stdout */
	if(annoy)
		show_log(1, annoy_types);

}

void vsystem_log(const char *msg, char *module, char *func, int msg_type, ...) {
	static char msg_buf[MAX_MSG_LENGTH];
	va_list va;

	va_start(va, msg_type);
	vsnprintf(msg_buf, MAX_MSG_LENGTH, msg, va);
	va_end(va);

	system_log(msg_buf, module, func, msg_type);
}

void show_log(unsigned int count, bool *disp_types){
	int n, final_count, current;

	if(!serial){ /* if there are no messages to display */
		printk("No messages to display\n");
		return;
	}

	final_count = count < N_message ? count : N_message;
  current = (N_message - final_count)%N_DEBUG_MSG;

	for(n=0; n<final_count; n++){
		if(disp_types != NULL){
			if(disp_types[log[current].msg_type]) /* show only messages set to be displayed */
				printk("#%d  [%s][mod %s][func %s]: %s\n", log[current].serial,
							 types_str[log[current].msg_type], log[current].module, log[current].func,
							 log[current].msg);
		}else{  /* no specific information on output */
				printk("#%d  [%s][mod %s][func %s]: %s\n", log[current].serial,
							 types_str[log[current].msg_type], log[current].module, log[current].func,
							 log[current].msg);
		}
		current++;
		if(current>=N_DEBUG_MSG)
			current = 0;
	}
}
