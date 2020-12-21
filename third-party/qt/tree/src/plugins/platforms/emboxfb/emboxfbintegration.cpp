/**
 * @file
 * @brief Embox Framebuffer plugin
 *
 * @date 15.06.2019
 * @author Alexander Kalmuk
 */

#include "qgenericunixfontdatabase.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <qimage.h>
#include "emboxfbintegration.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/fb.h>

#define DPRINTF(fmt, ...) \
    do { \
        fprintf(stderr, "> QEmboxFB: " fmt, ## __VA_ARGS__); \
    } while (0)

void QEmboxFbIntegration::printFbInfo(void)
{
	DPRINTF("Framebuffer info:\n");
	DPRINTF("  Framebuffer start = 0x%x\n", (unsigned int) fbData);
	DPRINTF("  Width = %d, Height = %d\n", fbWidth, fbHeight);
	DPRINTF("  BytesPerLine = %d\n", fbBytesPerLine);
	DPRINTF("  Format = %d\n", fbFormat);
	DPRINTF("\n");
}

QEmboxFbIntegration::QEmboxFbIntegration()
	: fontDb(new QGenericUnixFontDatabase())
{
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	const char *fbPath = "/dev/fb0";

	fbFd = open(fbPath, O_RDWR);
	if (fbFd < 0) {
		qFatal("QEmboxFbIntegration: Error open framebuffer %s", fbPath);
	}
	if (ioctl(fbFd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		qFatal("QEmboxFbIntegration: Error ioctl framebuffer %s", fbPath);
	}
	if (ioctl(fbFd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		qFatal("QEmboxFbIntegration: Error ioctl framebuffer %s", fbPath);
	}
	fbWidth        = vinfo.xres;
	fbHeight       = vinfo.yres;
	fbBytesPerLine = finfo.line_length;
	fbSize         = fbBytesPerLine * fbHeight;
	fbFormat       = vinfo.fmt;
	fbData = (uint8_t *)mmap(0, fbSize, PROT_READ | PROT_WRITE,
				             MAP_SHARED, fbFd, 0);
	if (fbData == MAP_FAILED) {
		qFatal("QEmboxFbIntegration: Error mmap framebuffer %s", fbPath);
	}
	if (!fbData || !fbSize) {
		qFatal("QEmboxFbIntegration: Wrong framebuffer: base = %p,"
			   "size=%d", fbData, fbSize);
	}

	mPrimaryScreen = new QEmboxFbScreen(fbData, fbWidth,
							            fbHeight, fbBytesPerLine,
							            emboxFbFormatToQImageFormat(fbFormat));

	mPrimaryScreen->setPhysicalSize(QSize(fbWidth, fbHeight));
	mScreens.append(mPrimaryScreen);

	this->printFbInfo();
}

QEmboxFbIntegration::~QEmboxFbIntegration()
{
	delete mPrimaryScreen;
}

FbFormat QEmboxFbIntegration::emboxFbFormatToQImageFormat(
	int emboxFbFormat)
{
	FbFormat format;

	format.format = QImage::Format_Invalid;
	format.swapRgb = 0;

	switch (emboxFbFormat) {
	case EMBOX_BGR888:
		format.swapRgb = 1;
	case EMBOX_RGB888:
		format.format =  QImage::Format_RGB888;
		break;
	case EMBOX_BGRA8888:
		format.swapRgb = 1;
	case EMBOX_RGBA8888:
		format.format = QImage::Format_ARGB32;
		break;
	case EMBOX_BGR565:
		format.swapRgb = 1;
	case EMBOX_RGB565:
		format.format = QImage::Format_RGB16;
		break;
	default:
		qFatal("QEmboxFbIntegration: Unknown framebuffer format %d",
			emboxFbFormat);
		break;
	}
	return format;
}

bool QEmboxFbIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
	switch (cap) {
	case ThreadedPixmaps:
		return true;
	default:
		return QPlatformIntegration::hasCapability(cap);
	}
}

QPixmapData *QEmboxFbIntegration::createPixmapData(QPixmapData::PixelType type) const
{
	return new QRasterPixmapData(type);
}

QPlatformWindow *QEmboxFbIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
	Q_UNUSED(winId);
	QFbWindow *w = new QFbWindow(widget);
	mPrimaryScreen->addWindow(w);
	return w;
}

QWindowSurface *QEmboxFbIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
	Q_UNUSED(winId);
	return new QFbWindowSurface(mPrimaryScreen, widget);
}

QPlatformFontDatabase *QEmboxFbIntegration::fontDatabase() const
{
	return fontDb;
}

QEmboxFbScreen::QEmboxFbScreen(uint8_t *data, int width,
	int height, int bytesPerLine,
	FbFormat format) : compositePainter(0)
{
	mData         = data;
	mGeometry     = QRect(0, 0, width, height);
	mBytesPerLine = bytesPerLine;
	mFormat       = format.format;
	mSwapRgb      = format.swapRgb;
	mDepth        = 16;

	mScreenImage = new QImage(mGeometry.width(), mGeometry.height(), mFormat);
	mFbScreenImage = new QImage(mData, mGeometry.width(), mGeometry.height(),
						        mBytesPerLine, mFormat);
#ifndef QT_NO_CURSOR
    cursor = new QPlatformSoftwareCursor(this);
#endif
}

void QEmboxFbScreen::setGeometry(QRect rect)
{
	mGeometry = rect;

	delete mFbScreenImage;
	mFbScreenImage = new QImage(mData, mGeometry.width(), mGeometry.height(),
						        mBytesPerLine, mFormat);
	delete compositePainter;
	compositePainter = 0;	
	delete mScreenImage;
	mScreenImage = new QImage(mGeometry.width(), mGeometry.height(),
					          mFormat);
}

void QEmboxFbScreen::setFormat(QImage::Format format)
{
	mFormat = format;

	delete mFbScreenImage;
	mFbScreenImage = new QImage(mData, mGeometry.width(), mGeometry.height(),
						        mBytesPerLine, mFormat);
	delete compositePainter;
	compositePainter = 0;
	delete mScreenImage;
	mScreenImage = new QImage(mGeometry.width(), mGeometry.height(),
						      mFormat);
}

QRegion QEmboxFbScreen::doRedraw()
{
	QVector<QRect> rects;
	QRegion touched = QFbScreen::doRedraw();
	
	if (!compositePainter) {
		compositePainter = new QPainter(mFbScreenImage);
	}
	
	rects = touched.rects();
	if (mSwapRgb) {
		for (int i = 0; i < rects.size(); i++) {
			compositePainter->drawImage(rects[i], mScreenImage->rgbSwapped(), rects[i]);
			}
	} else {
		for (int i = 0; i < rects.size(); i++) {
			compositePainter->drawImage(rects[i], *mScreenImage, rects[i]);
		}
	}
	return touched;
}
