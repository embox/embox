#ifndef QPLATFORMINTEGRATION_MINIMAL_H
#define QPLATFORMINTEGRATION_MINIMAL_H

#include <QtGui>
#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>
#include <../fb_base/fb_base.h>
#include "qgenericunixfontdatabase.h"

QT_BEGIN_NAMESPACE

class QEmboxVCScreen : public QPlatformScreen
{
public:
    QEmboxVCScreen()
        : mDepth(32), mFormat(QImage::Format_ARGB32_Premultiplied) {}

    QRect geometry() const { return mGeometry; }
    int depth() const { return mDepth; }
    QImage::Format format() const { return mFormat; }

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
};

class QEmboxVCIntegration : public QPlatformIntegration
{
public:
    QEmboxVCIntegration();

    bool hasCapability(QPlatformIntegration::Capability cap) const;

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const;
    QPlatformWindow *createPlatformWindow(QWidget *widget, WId winId) const;
    QWindowSurface *createWindowSurface(QWidget *widget, WId winId) const;

    QPixmap grabWindow(WId window, int x, int y, int width, int height) const;

    QList<QPlatformScreen *> screens() const { return mScreens; }

    QPlatformFontDatabase *fontDatabase() const;

private:
    QList<QPlatformScreen *> mScreens;
    QPlatformFontDatabase *fontDb;
};

QT_END_NAMESPACE

#endif
