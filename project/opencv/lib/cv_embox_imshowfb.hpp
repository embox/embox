/**
 * @file
 *
 * @date   12.04.2021
 * @author Alexander Kalmuk
 */

#ifndef PROJECT_OPENCV_LIB_CV_EMBOX_IMSHOWFB_HPP
#define PROJECT_OPENCV_LIB_CV_EMBOX_IMSHOWFB_HPP

#ifndef __cplusplus
#  error cv_embox_imshowfb.hpp header must be compiled as C++
#endif

#include <opencv2/core/mat.hpp>

/* It's an analogue of OpenCV imshow(), which draws image directly
 * to the framebuffer instead of a graphical window. It makes easy
 * to see the resulting image without underlaying window system.
 *
 * It means that all usages of imshow() in the application have to
 * be replaced with imshowfb(). */
extern void imshowfb(cv::Mat& img, int fbx);

#endif /* PROJECT_OPENCV_LIB_CV_EMBOX_IMSHOWFB_HPP */
