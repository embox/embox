#include "emboxvcintegration.h"
#include "emboxvcwindowsurface.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>
#include <QtGui/QGraphicsView>

QEmboxVC *globalEmboxVC;

QEmboxVCIntegration::QEmboxVCIntegration()
    : fontDb(new QGenericUnixFontDatabase())
{
    QEmboxVCScreen *mPrimaryScreen = new QEmboxVCScreen();

    mPrimaryScreen->mGeometry = QRect(0, 0, 1024, 768);
    mPrimaryScreen->mDepth = 16;
    mPrimaryScreen->mFormat = QImage::Format_RGB16;

    globalEmboxVC = new QEmboxVC();

    mScreens.append(mPrimaryScreen);
}

bool QEmboxVCIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPixmapData *QEmboxVCIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QPlatformWindow *QEmboxVCIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
	Q_UNUSED(winId);

	QEmboxVCPlatformWindow *window = new QEmboxVCPlatformWindow(widget);
	return window;
}

QWindowSurface *QEmboxVCIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QEmboxVCWindowSurface(widget);
}

QPixmap QEmboxVCIntegration::grabWindow(WId window, int x, int y, int width, int height) const
{
	return QPixmap();
}

QPlatformFontDatabase *QEmboxVCIntegration::fontDatabase() const
{
    return fontDb;
}
