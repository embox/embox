#include "emboxvcintegration.h"
#include "emboxvcwindowsurface.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

QEmboxVCIntegration::QEmboxVCIntegration()
{
    QEmboxVCScreen *mPrimaryScreen = new QEmboxVCScreen();

    mPrimaryScreen->mGeometry = QRect(0, 0, 1024, 768);
    mPrimaryScreen->mDepth = 16;
    mPrimaryScreen->mFormat = QImage::Format_RGB16;

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
    return new QPlatformWindow(widget);
}

QWindowSurface *QEmboxVCIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QEmboxVCWindowSurface(widget);
}
