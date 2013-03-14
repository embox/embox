#include "emboxvcwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE

static QList<QEmboxVCWindowSurface*> __emboxVCcollection;

static void __emboxVCsetMode(struct vc *vc, int mode) {
	for (int i = 0; i < __emboxVCcollection.size(); ++i) {
		if (&(__emboxVCcollection.at(i)->emboxVC) == vc) {
			__emboxVCcollection.at(i)->emboxVCvisualized = mode;
		}
	}
}

static void __visualization(struct vc *vc, struct fb_info *info) {
	Q_UNUSED(info);
	printf(">>__visualization\n");
	__emboxVCsetMode(vc, 1);
}

static void __scheduleDevisualization(struct vc *vc) {
	printf(">>__scheduleDevisualization\n");
	__emboxVCsetMode(vc, 0);
}

QEmboxVCWindowSurface::QEmboxVCWindowSurface(QWidget *window)
    : QWindowSurface(window), emboxVCvisualized(1) /* XXX it is so? */
{

	printf(">>QEmboxVCWindowSurface begin 2\n");

	emboxVCcallbacks.visualized = __visualization;
	emboxVCcallbacks.schedule_devisualization = __scheduleDevisualization;

	emboxVC.fb = fb_lookup("fb0");
	printf("emboxFB - %p\n", emboxVC.fb);
	emboxVC.callbacks = &emboxVCcallbacks;
	emboxVC.name = "emboxvc";

	printf("mpx_register_vc return %d\n", mpx_register_vc(&emboxVC));
	__emboxVCcollection.append(this);
	printf(">>QEmboxVCWindowSurface end\n");
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

    if (!emboxVCvisualized) {
    	printf(">>flush: !emboxVCvisualized\n");
    	return;
    }

    printf(">>flush\n");

    for (i = 0, shift = 0; i < mImage.height(); i++ , shift += mImage.bytesPerLine()) {
    	memcpy(emboxVC.fb->screen_base + shift, (const void *)mImage.constScanLine(i), mImage.bytesPerLine());
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
