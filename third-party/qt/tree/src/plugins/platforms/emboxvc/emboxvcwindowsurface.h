#ifndef QWINDOWSURFACE_MINIMAL_H
#define QWINDOWSURFACE_MINIMAL_H

#include <QtGui/private/qwindowsurface_p.h>

#include <QtGui/QPlatformWindow>
#include <drivers/video/fb.h>
#include <string.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

class QEmboxVCWindowSurface : public QWindowSurface
{
public:
    QEmboxVCWindowSurface(QWidget *window);
    ~QEmboxVCWindowSurface();

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

private:
    QImage mImage;
    struct fb_info *emboxFB;
};

QT_END_NAMESPACE

#endif
