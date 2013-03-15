#include "emboxvcwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

QT_BEGIN_NAMESPACE

static QList<QEmboxVCWindowSurface*> __emboxVCcollection;

static QEmboxVCWindowSurface * __emboxVC(struct vc *vc) {
	for (int i = 0; i < __emboxVCcollection.size(); ++i) {
		QEmboxVCWindowSurface *emvc = __emboxVCcollection.at(i);
		if (&(emvc->emboxVC) == vc) {
			return emvc;
		}
	}

	return NULL;
}

static void __emboxVCsetMode(struct vc *vc, int mode) {
	(__emboxVC(vc))->emboxVCvisualized = mode;
}

static void __visualization(struct vc *vc, struct fb_info *info) {
	Q_UNUSED(info);
	QRegion region;
	QPoint point;

	QEmboxVCWindowSurface *surf;

	printf(">>__visualization\n");
	__emboxVCsetMode(vc, 1);
	surf = __emboxVC(vc);
	surf->flush(NULL, region, point);
	printf(">>done __visualization\n");
}

static void __handle_input_event(struct vc *vc, struct input_event *ev) {

}

static void __scheduleDevisualization(struct vc *vc) {
	printf(">>__scheduleDevisualization\n");
	__emboxVCsetMode(vc, 0);

	mpx_devisualized(vc);
}

QEmboxVCWindowSurface::QEmboxVCWindowSurface(QWidget *window)
    : QWindowSurface(window), emboxVCvisualized(0)
{

	printf(">>QEmboxVCWindowSurface begin 2\n");

        mImage = QImage(QSize(1024, 768), QImage::Format_RGB16);

	emboxVCcallbacks.visualized = __visualization;
	emboxVCcallbacks.schedule_devisualization = __scheduleDevisualization;
	emboxVCcallbacks.handle_input_event = __handle_input_event;

	emboxVC.callbacks = &emboxVCcallbacks;
	emboxVC.name = "emboxvc";

	__emboxVCcollection.append(this);
	printf("mpx_register_vc return %d\n", mpx_register_vc(&emboxVC));
	printf(">>QEmboxVCWindowSurface end\n");
}

QEmboxVCWindowSurface::~QEmboxVCWindowSurface()
{

}

QPaintDevice *QEmboxVCWindowSurface::paintDevice()
{
    return &mImage;
}

void QEmboxVCWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    int shift, i;

    if (!emboxVC.fb || !emboxVCvisualized) {
    	return;
    }

    for (i = 0, shift = 0; i < mImage.height(); i++ , shift += mImage.bytesPerLine()) {
    	memcpy(emboxVC.fb->screen_base + shift, (const void *)mImage.constScanLine(i), mImage.bytesPerLine());
    }
}

void QEmboxVCWindowSurface::resize(const QSize &size)
{
    //qDebug() << "QMinimalWindowSurface::setGeometry:" << (long)this << rect;
    //QWindowSurface::resize(size);
    //QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();
    //if (mImage.size() != size)
            //[> XXX <]
        //mImage = QImage(size, QImage::Format_RGB16);
}

QT_END_NAMESPACE
