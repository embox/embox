/**
 * @file version.cpp
 * @brief Print OpenCV build info
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.06.2019
 */

#include <stdio.h>
#include <opencv2/core/utility.hpp>

int main() {
	printf("OpenCV: %s", cv::getBuildInformation().c_str());

	return 0;
}
