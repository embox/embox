/**
 * @file
 *
 * @date 01.12.25
 * @author Anton Bondarev
 */

#ifndef _UAPI__LINUX_V4L2_SUBDEV_H
#define _UAPI__LINUX_V4L2_SUBDEV_H

#include <stdint.h>

#include <sys/ioctl.h>

/******************** FROM uapi/linux/v4l2-mediabus.h */

struct v4l2_mbus_framefmt {
	uint32_t			width;
	uint32_t			height;
	uint32_t			code;
	uint32_t			field;
	uint32_t			colorspace;
	uint16_t			ycbcr_enc;
	uint16_t			quantization;
	uint16_t			xfer_func;
	uint16_t			reserved[11];
};
/* ******************* END uapi/linux/v4l2-mediabus.h */

enum v4l2_subdev_format_whence {
	V4L2_SUBDEV_FORMAT_TRY = 0,
	V4L2_SUBDEV_FORMAT_ACTIVE = 1,
};

#define VIDIOC_SUBDEV_QUERYCAP			_IOR('V',  0, struct v4l2_subdev_capability)
#define VIDIOC_SUBDEV_G_FMT			_IOWR('V',  4, struct v4l2_subdev_format)
#define VIDIOC_SUBDEV_S_FMT			_IOWR('V',  5, struct v4l2_subdev_format)
#define VIDIOC_SUBDEV_G_FRAME_INTERVAL		_IOWR('V', 21, struct v4l2_subdev_frame_interval)
#define VIDIOC_SUBDEV_S_FRAME_INTERVAL		_IOWR('V', 22, struct v4l2_subdev_frame_interval)
#define VIDIOC_SUBDEV_ENUM_MBUS_CODE		_IOWR('V',  2, struct v4l2_subdev_mbus_code_enum)
#define VIDIOC_SUBDEV_ENUM_FRAME_SIZE		_IOWR('V', 74, struct v4l2_subdev_frame_size_enum)
#define VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL	_IOWR('V', 75, struct v4l2_subdev_frame_interval_enum)
#define VIDIOC_SUBDEV_G_CROP			_IOWR('V', 59, struct v4l2_subdev_crop)
#define VIDIOC_SUBDEV_S_CROP			_IOWR('V', 60, struct v4l2_subdev_crop)
#define VIDIOC_SUBDEV_G_SELECTION		_IOWR('V', 61, struct v4l2_subdev_selection)
#define VIDIOC_SUBDEV_S_SELECTION		_IOWR('V', 62, struct v4l2_subdev_selection)
#define VIDIOC_SUBDEV_G_ROUTING			_IOWR('V', 38, struct v4l2_subdev_routing)
#define VIDIOC_SUBDEV_S_ROUTING			_IOWR('V', 39, struct v4l2_subdev_routing)
#define VIDIOC_SUBDEV_G_CLIENT_CAP		_IOR('V',  101, struct v4l2_subdev_client_capability)
#define VIDIOC_SUBDEV_S_CLIENT_CAP		_IOWR('V',  102, struct v4l2_subdev_client_capability)

struct v4l2_subdev_format {
	uint32_t which;
	uint32_t pad;
	struct v4l2_mbus_framefmt format;
	uint32_t reserved[8];
};

#endif /* _UAPI__LINUX_V4L2_SUBDEV_H */