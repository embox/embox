/**
 * @file
 * @brief Here is the entry point for XWnd.
 *
 * @date Oct 1, 2012
 * @author Alexandr Chernakov
 */


#include <stdio.h>
#include <kernel/time/timer.h>
#include <kernel/task.h>
#include <drivers/vesa.h>
#include <drivers/keyboard.h>
#include <embox/cmd.h>
#include <xwnd/xwnd.h>
#include <xwnd/bmp.h>

EMBOX_CMD(exec);

struct xwnd_app_pipe {
	int input;
	int output;
};

struct xwnd_app_pipes_array {
	int count;
	int allocated;
	struct xwnd_app_pipe * array;
};

struct xwnd_app_pipes_array * xwnd_pipes_init(void);
const struct xwnd_app_pipe * xwnd_pipe_add(void);
void xwnd_pipe_delete(void);

int xwnd_init(void);
int xwnd_start(void);
int xwnd_widgets_init(void);
void xwnd_quit(void);

int xwnd_send_app_msg(void);

int xwnd_init() {
	vesa_init_mode(VESA_MODE_DEFAULT);
	vesa_clear_screen();
	return 0;
}
void xwnd_quit(){
	vesa_quit_mode();
}

void * test_task(void * arg) {
	int * pipefd = (int*)arg;
	int x = 0;
	read (pipefd[0], &x, sizeof(int));
	printf("%d\n", x);
	return NULL;
}

static int exec (int argc, char ** argv) {
	struct xwnd_bmp_image * img;

	if (argc > 1) {
		img = xwnd_bmp_load(argv[1]);
		if (!img) {
			printf ("Failed to load bmp, error code %d\n", xwnd_bmp_get_errno());
			return 1;
		}

		if (argc == 2) {
			xwnd_init();

			xwnd_bmp_draw(img);
			xwnd_bmp_unload(img);
			while (1) {
				if (!keyboard_has_symbol()) {
					usleep(100);
				}
				else if ('q' == keyboard_getc()) {
					break;
				}
			}

			xwnd_quit();
			return 0;
		}
		if (argc > 2)
		{
			int pipefd[2];
			int tid;
			int x = 10;

			printf("w/h: %d/%d, bpp: %d, ERR: %d\n", img->width, img->height, img->bpp, xwnd_bmp_get_errno());
			xwnd_bmp_unload(img);

			pipe(pipefd);
			write(pipefd[1], &x, sizeof(int));
			tid = new_task(test_task, pipefd, 0);
			tid = tid;
			return 0;
		}
	}
	else {
		printf("Need more arguments\n");
		return 1;
	}
	return 1;
}
