/**
 * @file
 *
 * @date 01.04.10
 * @author Anton Kozlov
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <drivers/vt.h>
#include <kernel/diag.h>
#include <unistd.h>
#include <kernel/timer.h>

EMBOX_CMD(exec);

#if 0x0 /* fix warnings: function print_usage not used */
static void print_usage(void) {
	printf("Usage: snake\n");
}
#endif

#ifndef ESC
#define ESC 0x1b
#endif
#ifndef CSI
#define CSI '['
#endif

#define SPEED_INC 5
#define SIZE 18
#define SNAKE_LEN 4
#define SNK '*'
#define FRU 'o'

static int rand_seed = 0;
static int sleep_time = 350;
static int speed_count = SPEED_INC;

static char field[SIZE * SIZE];
static char dir[SIZE * SIZE];

typedef struct {
	int x;
	int y;
} point;

static point head, tail, diff;
static char diff_char = 'l';

static point dxp = {1,0};
static point dxm = {-1,0};
static point dyp = {0,1};
static point dym = {0,-1};

static point nil = {0,0};

static int score = 0;

static int f2d(int x, int y) {
	return x + y * SIZE;
}

static point d2f(int offset) {
	point p = {offset % SIZE, offset / SIZE};
	return p;
}


static void gotoxy(point p) {
	printf("%c%c%d;%dH",ESC,CSI,p.y+2,p.x+1);
}

static void gotohome(void) {
	printf("%c%cH",ESC,CSI);
}

static void snake_init(void) {
	int j = SIZE/2;
	int si = (SIZE-SNAKE_LEN) /2;
	int i;
	head.y = tail.y = j;
	tail.x = si;
	for (i = 0; i < SNAKE_LEN; i++) {
		field[f2d(si + i, j)] = SNK;
		dir  [f2d(si + i, j)] = 'l';
	}
	head.x = si + i - 1;
}

static void draw(point p, char c) {
	gotoxy(p);
	printf("%c",c);
	field[f2d(p.x, p.y)] = c;
}

static int space(char c) {
	return (c == '.' || c == ' ');
}

static int random(void) {
	rand_seed += cnt_system_time();
	return rand_seed;
}

static void fruit_draw(void) {
	int t = random() % (SIZE * SIZE);
	while (!space(field[t]))
		t = random() % (SIZE * SIZE);
	draw(d2f(t), FRU);
	gotohome();
}

static void field_init(void) {
	int i, j;
	printf("%c%c2J",ESC,CSI);
	gotohome();
	printf("Score: 0\n");
	for (i = 1; i < SIZE - 1; i++) {
		field[f2d(i, 0)] = field[f2d(i, SIZE-1)] = '-';
		field[f2d(0, i)] = field[f2d(SIZE-1, i)] = '|';
	}
	field[f2d(0,0)] = field[f2d(0,SIZE-1)] = \
		      field[f2d(SIZE-1,0)] = field[f2d(SIZE-1,SIZE-1)] = '+';
	for (i=1;i<SIZE-1;i++)
		for (j=1;j<SIZE-1;j++)
			field[f2d(i,j)] = '.';
	snake_init();
	for (i=0;i<SIZE;i++) {
		for (j=0;j<SIZE;j++)
			printf("%c",field[f2d(j,i)]);
		putchar('\n');
	}
	fruit_draw();
}

static point point_plus(point p1, point p2) {
	point p = {p1.x + p2.x, p1.y + p2.y};
	return p;
}

static char field_char(point p) {
	return field[f2d(p.x,p.y)];
}

static char dir_char(point p) {
	return dir[f2d(p.x,p.y)];
}

static point dispatch(char c) {
	switch (c) {
	case 'h':
		return dxm;
	case 'j':
		return dyp;
	case 'k':
		return dym;
	case 'l':
		return dxp;
	}
	return nil;
}

#if 0
static int pcmp(point p1, point p2) {
	return p1.x - p2.x + p1.y - p2.y;
}
#endif

static int snake_update(void) {
	point p = point_plus(head, diff);
	char new_pos = field_char(p);

	dir[f2d(head.x, head.y)] = diff_char;

	switch (new_pos) {
	case FRU:
		if (!speed_count--) {
			if (sleep_time >= 50)
				sleep_time -= 20;
			speed_count = SPEED_INC;
		}
		head = p;
		score += 10;
		draw(p,SNK);
		fruit_draw();
		gotohome();
		printf("Score: %d",score);
		break;
	case ' ':
	case '.':
		draw(tail,'.');
		draw(p, SNK);
		head = p;
		tail = point_plus(dispatch(dir_char(tail)), tail);
		break;
	default:
		draw(p,'X');
		gotoxy(p);
		return 0;
	}
	gotohome();
	return 1;
}

static int last_valid;

static int valid(point p) {
	return (last_valid = !(p.x == 0 && p.y == 0));
}

static int exec(int argc, char **argv) {
	field_init();
	diff = dxp;
	getchar();
	while (snake_update()) {
		point d = nil;
		point d2 = nil;
		char ch = ' ';

		usleep(sleep_time);
		last_valid = 0;
		while (diag_has_symbol() && !last_valid ) {
			d = dispatch((ch = getchar()));
			d2 = point_plus(d,diff);
			valid(d2);
		}
		if (last_valid) {
			diff = d;
			diff_char = ch;
		}
		while (diag_has_symbol())
			getchar();
	}
	getchar();
	printf("%c%c2J",ESC,CSI);
	gotohome();
	return 0;
}
