#include "emboxvcwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE

QEmboxVCWindowSurface::QEmboxVCWindowSurface(QWidget *window)
    : QWindowSurface(window)
{
	emboxFB = fb_lookup("fb0");
	printf("emboxFB - %p\n", emboxFB);
    //qDebug() << "QMinimalWindowSurface::QMinimalWindowSurface:" << (long)this;
}

QEmboxVCWindowSurface::~QEmboxVCWindowSurface()
{
}

QPaintDevice *QEmboxVCWindowSurface::paintDevice()
{
    //qDebug() << "QMinimalWindowSurface::paintDevice";
    return &mImage;
}

void QEmboxVCWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    int shift, i;

    //static int c = 0;
    //QString filename = QString("output%1.png").arg(c++, 4, 10, QLatin1Char('0'));
    //qDebug() << "QMinimalWindowSurface::flush() saving contents to" << filename.toLocal8Bit().constData();
    //qDebug() << "emboxvc";
    //mImage.copy();
    //mImage.save(filename);
    for (i = 0, shift = 0; i < mImage.height(); i++ , shift += mImage.bytesPerLine()) {
    	memcpy(emboxFB->screen_base + shift, (const void *)mImage.constScanLine(i), mImage.bytesPerLine());
    }
}

void QEmboxVCWindowSurface::resize(const QSize &size)
{
    //qDebug() << "QMinimalWindowSurface::setGeometry:" << (long)this << rect;
    QWindowSurface::resize(size);
    QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();
    if (mImage.size() != size)
    	/* XXX */
        mImage = QImage(size, QImage::Format_RGB16);
}

QT_END_NAMESPACE
