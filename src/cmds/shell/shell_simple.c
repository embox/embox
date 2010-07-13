/**
 */
#include <stdio.h>
#include <shell.h>
#include <embox/unit.h>
#include <shell_command.h>
#include <driver.h>

EMBOX_UNIT(shell_start, shell_stop);

static int shell_start(void) {
	stdin = stdout = device_select( CONFIG_DEV_STDIO );
	printf("stdio: %d %d \n", stdin, stdout );

	printf("\n\n%s", CONFIG_SHELL_WELCOME_MSG);

	printf("\n\n\e[1;34m Just for fun MESSAGE :D\e[0;0m\n\n");

	printf("Test added fi_uart device\n");

	while (true) {
		int tmp;
		tmp=fgetc(stdin);
		printf("%d ",tmp);
		fputc(stdout,tmp);
		if (tmp==10) break;
	}

	printf("\n\e[1;34mTest is done.\e[0;0m\n");

	while (true) {
		int tmp;
		tmp=getchar();
		printf("%d ",tmp);
		putchar(tmp);
		if (tmp==10) break;
	}
	return 0;
}

static int shell_stop(void) {

	return 0;
}

