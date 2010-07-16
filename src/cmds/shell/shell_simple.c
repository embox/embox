/**
 */
#include <stdio.h>
#include <shell.h>
#include <embox/unit.h>
#include <shell_command.h>
#include <driver.h>

EMBOX_UNIT(shell_start, shell_stop);

static int shell_start(void) {
	FILE dev;
	dev = device_select( CONFIG_DEV_STDIO );
	stdin = stdout = dev;
	printf("\n\n%s", CONFIG_SHELL_WELCOME_MSG);
	printf("\n\n\e[1;34m Just for fun MESSAGE :D\e[0;0m\n\n");

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







	while (true) {
		int tmp;
		tmp=getchar();
		#if 0
		printf("%d ",tmp);
		#endif
		putchar(tmp);
		if (tmp==10) break;
	}
	return 0;
}

static int shell_stop(void) {

	return 0;
}

