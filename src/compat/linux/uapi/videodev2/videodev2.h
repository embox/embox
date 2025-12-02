/**
 * @file
 *
 * @date 01.12.25
 * @author Anton Bondarev
 */

#ifndef _UAPI__LINUX_VIDEODEV2_H
#define _UAPI__LINUX_VIDEODEV2_H


enum v4l2_memory {
	V4L2_MEMORY_MMAP             = 1,
	V4L2_MEMORY_USERPTR          = 2,
	V4L2_MEMORY_OVERLAY          = 3,
	V4L2_MEMORY_DMABUF           = 4,
};

#endif /* _UAPI__LINUX_VIDEODEV2_H */