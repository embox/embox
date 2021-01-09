#ifndef __MAIN_H__
#define __MAIN_H__

enum call_state {
	CALL_INACTIVE,
	CALL_INCOMING,
	CALL_ACTIVE,
};

struct demo_call_info {
	enum call_state state;

	char incoming[128];
	char remote_uri[128];
	char remote_contact[128];

	int redraw_required;
};

extern int demo_pj_main(int argc, char *argv[], int (*nk_cb)(const char *),
	const char *input_dev_path);
extern void demo_pj_answer(void);
extern void demo_pj_hang(void);

extern int demo_nk_main(const char *input_dev_path);
extern void demo_nk_on_incoming_call(void);

#define RAWFB_FONT_32       0
#define RAWFB_FONT_DEFAULT  RAWFB_FONT_32
//#define RAWFB_FONT_32     1
#define RAWFB_FONT_COUNT  1
#define RAWFB_FONT_SIZES \
    { \
        [RAWFB_FONT_32] = 32.0f, \
        [RAWFB_FONT_COUNT] = 0, \
    }


#endif /* __MAIN_H__ */
