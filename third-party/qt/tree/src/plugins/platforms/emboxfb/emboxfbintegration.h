/**
 * @file
 * @brief Embox Framebuffer plugin
 *
 * @date 15.06.2019
 * @author Alexander Kalmuk
 */

#ifndef QPLATFORMINTEGRATION_EMBOXBOXFb_H
#define QPLATFORMINTEGRATION_EMBOXBOXFb_H

#include <QPlatformIntegration>
#include "../fb_base/fb_base.h"

QT_BEGIN_NAMESPACE

class QEmboxFbScreen : public QFbScreen
{
	Q_OBJECT
public:
	QEmboxFbScreen(uint8_t *d, int width, int height,
					int stride, QImage::Format screenFormat);
	void setGeometry(QRect rect);
	void setFormat(QImage::Format format);

public slots:
	QRegion doRedraw();

private:
	uint8_t *mData;
	QImage *mFbScreenImage;
	unsigned int mBytesPerLine;

	QPainter *compositePainter;
};

class QEmboxFbIntegration : public QPlatformIntegration
{
public:
	QEmboxFbIntegration();
	~QEmboxFbIntegration();

	bool hasCapability(QPlatformIntegration::Capability cap) const;

	QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
	QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
	QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

	QList<QPlatformScreen *> screens() const { return mScreens; }

	QPlatformFontDatabase *fontDatabase() const;

private:
	enum { UNKNOWN = 0, EMBOX_RGB888, EMBOX_BGR888, EMBOX_RGBA8888,
		   EMBOX_BGRA8888, EMBOX_RGB565, EMBOX_BGR565 };

	void           printFbInfo(void);
	QImage::Format emboxFbFormatToQImageFormat(int emboxFbFormat);

	int fbFd;

	uint8_t *fbData; /* Memory mapped framebuffer */
	unsigned int fbSize;
	unsigned int fbWidth;
	unsigned int fbHeight;
	unsigned int fbBytesPerLine;
	int fbFormat;

	QEmboxFbScreen *mPrimaryScreen;
	QList<QPlatformScreen *> mScreens;
	QPlatformFontDatabase *fontDb;
};

QT_END_NAMESPACE

#endif
