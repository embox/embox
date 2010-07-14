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
	printk("before select device!\n");
	dev = device_select( CONFIG_DEV_STDIO );
	printk("selected device == %d \n",dev);
	stdin = stdout = dev;

	printk("stdio: %d %d \n", stdin, stdout );

	printk("\n\n%s", CONFIG_SHELL_WELCOME_MSG);

	printk("\n\n\e[1;34m Just for fun MESSAGE :D\e[0;0m\n\n");

	printk("Test added fi_uart device\n");

	while (true) {
		int tmp;
		tmp=fgetc(stdin);
		printk("%d ",tmp);
		fputc(stdout,tmp);
		if (tmp==10) break;
	}

	printk("\n\e[1;34mTest is done.\e[0;0m\n");

	while (true) {
		int tmp;
		tmp=getchar();
		printk("%d ",tmp);
		putchar(tmp);
		if (tmp==10) break;
	}
	return 0;
}

static int shell_stop(void) {

	return 0;
}

