/**
 * @file
 * @brief
 *
 * @date 20.03.13
 * @author Vita Loginova
 */

#ifndef IODEV_DIAG_H_
#define IODEV_DIAG_H_

typedef struct vchar {
	char c;
	char a;
}__attribute__((packed)) vchar_t;

struct diag_vterm_data {
	char attr;
	vchar_t *video;
};

struct vga_vterm_video {
	struct vterm_video video;
	struct diag_vterm_data data;
};

#endif /* IODEV_DIAG_H_ */
