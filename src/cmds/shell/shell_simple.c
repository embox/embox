/**
 */
#include <stdio.h>
#include <shell.h>
#include <embox/unit.h>
#include <driver.h>
#include <kernel/thread.h>
#include <kernel/scheduler.h>
#include <kernel/printk.h>

EMBOX_UNIT(shell_start, shell_stop);

static int shell_start(void) {
	FILE dev;
	dev = device_select( CONFIG_DEV_STDIO );
	stdin = stdout = dev;
	printf("\n\n%s", CONFIG_SHELL_WELCOME_MSG);
	printf("\n\n\e[1;34m Just for fun MESSAGE :D\e[0;0m\n\n");


/* bind iTerminal with fi_uart */
	device_desc stdio, iterminal;
	//scheduler_start();
	printk("scheduler_start!!!\n");
	iterminal = device_select( "dev_itty01" );
	printk("id of itty01: %d\n",iterminal);
	stdio = device_select( CONFIG_DEV_STDIO );
	device_devctl( iterminal , 0x101 , &stdio );
/* */

#if 0 /* some code that may write and run all command */
	SHELL_COMMAND_DESCRIPTOR *scd;
	int return_code;
	char c;

		scd = shell_command_descriptor_find_first("help",- 1);
		printf("\e[0;33mrun now: %s\e[0;0m\n",scd->name);
		return_code = shell_command_exec(scd,0,NULL);
		printf("\e[0;33mreturn code: %d\e[0;0m\n\n",return_code);

	for (c='a';c<='z';++c) {
		printf("\e[1;35mRun all commands beginning on char: %c\e[0;0m\n",c);
		if (!(scd = shell_command_descriptor_find_first(&c,1))) {
			continue;
		}
		printf("\e[0;33mrun now: %s\e[0;0m\n",scd->name);
		return_code = shell_command_exec(scd,0,NULL);
		printf("\e[0;33mreturn code: %d\e[0;0m\n\n",return_code);

		while (NULL!=(scd = shell_command_descriptor_find_next(scd,&c,1))) {
			printf("%ld\n",scd);
			printf("\e[0;33mrun now: %s\e[0;0m\n",scd->name);
			return_code = shell_command_exec(scd,0,NULL);
			printf("\e[0;33mreturn code: %d\e[0;0m\n\n",return_code);
		}
	}
#endif

#if 0
	int arg = IOCTLP_NONBLOCKINGIO;
	device_ioctl( dev , IOCTL_SET_BASE_OPTIONS , &arg );

	while (true) {
		char ch = '\0';
		if (device_read( dev , &ch , 1 )>=0) {
			device_write( dev , &ch , 1 );
			printk(" %d\n", (int)ch);
		} else {
			printk("wait...\n");
		}
	}
return 0;
#endif


	while (true) {
		int tmp;
		tmp=getchar();
		putchar(tmp);
		#if 1
		printk(" main: %d\n",tmp);
		#endif
		if (tmp==10) break;
	}
	return 0;
}

static int shell_stop(void) {

	return 0;
}

