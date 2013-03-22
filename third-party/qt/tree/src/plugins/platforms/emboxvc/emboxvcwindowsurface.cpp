#include "emboxvcwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>
#include <QWindowSystemInterface>
#include <QMouseEvent>

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

	__emboxVCsetMode(vc, 1);
	surf = __emboxVC(vc);
	surf->flush(NULL, region, point);
}

QEmboxVCMouseHandler::QEmboxVCMouseHandler() {
	int pipefd[2];

	if (pipe(pipefd) < 0) {
		return;
	}

	mouseFD = pipefd[0];
	inputFD = pipefd[1];

	fcntl(mouseFD, F_SETFD, O_NONBLOCK);
	fcntl(inputFD, F_SETFD, O_NONBLOCK);

    mouseNotifier = new QSocketNotifier(mouseFD, QSocketNotifier::Read, this);
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
}

QEmboxVCMouseHandler::~QEmboxVCMouseHandler() {

}

void QEmboxVCMouseHandler::storeData(void *data, int datalen) {
	write(inputFD, data, datalen);
}

void QEmboxVCMouseHandler::readMouseData() {
	struct vc *vc;
	struct input_event ev;
	short x, y;
	int bstate;
	QEmboxVCWindowSurface *emvc;

	while (read(mouseFD, &vc, sizeof(struct vc *)) > 0) {

		read(mouseFD, &ev, sizeof(struct input_event));

		emvc = __emboxVC(vc);

		if (ev.devtype != INPUT_DEV_MOUSE) {
			return;
		}

		bstate = Qt::NoButton;

		if (ev.type & 1) {
			bstate = Qt::LeftButton;
		} else if (ev.type & 2) {
			bstate = Qt::RightButton;
		}

		x = ev.value >> 16;
		y = ev.value & 0xffff;

		emvc->mouseX += x;
		emvc->mouseY += -y;

		QWindowSystemInterface::handleMouseEvent(0, QPoint(emvc->mouseX, emvc->mouseY),
				QPoint(emvc->mouseX, emvc->mouseY), Qt::MouseButtons(bstate));

		int xres = emvc->emboxVC.fb->var.xres;
		int yres = emvc->emboxVC.fb->var.yres;

		emvc->mouseX = emvc->mouseX > 0 ? emvc->mouseX : 0;
		emvc->mouseY = emvc->mouseY > 0 ? emvc->mouseY : 0;

		emvc->mouseX = emvc->mouseX > xres ? xres : emvc->mouseX;
		emvc->mouseY = emvc->mouseY > yres ? yres : emvc->mouseY;

		if (!emvc->emboxVC.fb || !emvc->emboxVCvisualized) {
			return;
		}

		emvc->cursor->emboxCursorRedraw(emvc->emboxVC.fb, emvc->mouseX, emvc->mouseY);
	}
}

static void __handle_input_event(struct vc *vc, struct input_event *ev) {
	QEmboxVCWindowSurface *emvc = __emboxVC(vc);

	emvc->mouseHandler->storeData(&vc, sizeof(struct vc *));
	emvc->mouseHandler->storeData(ev, sizeof(struct input_event));
}

static void __scheduleDevisualization(struct vc *vc) {
	printf(">>__scheduleDevisualization\n");
	__emboxVCsetMode(vc, 0);

	mpx_devisualized(vc);
}

QEmboxVCWindowSurface::QEmboxVCWindowSurface(QWidget *window)
    : QWindowSurface(window), emboxVCvisualized(0), mouseX(0), mouseY(0)
{
	mImage = QImage(QSize(1024, 768), QImage::Format_RGB16);

	cursor = new QEmboxCursor();

	mouseHandler = new QEmboxVCMouseHandler();

	emboxVCcallbacks.visualized = __visualization;
	emboxVCcallbacks.schedule_devisualization = __scheduleDevisualization;
	emboxVCcallbacks.handle_input_event = __handle_input_event;

	emboxVC.callbacks = &emboxVCcallbacks;
	emboxVC.name = "emboxvc";

	__emboxVCcollection.append(this);
	mpx_register_vc(&emboxVC);
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

    int i, shift, bpp;

    if (!emboxVC.fb || !emboxVCvisualized) {
    	return;
    }

    bpp = emboxVC.fb->var.bits_per_pixel / 8;

    /* Draw image */
    for (i = 0, shift = 0; i < mImage.height(); i++ , shift += emboxVC.fb->var.xres * bpp) {
    	memcpy(emboxVC.fb->screen_base + shift, (const void *)mImage.constScanLine(i), mImage.bytesPerLine());
    }

    /* Draw cursor */
    cursor->emboxCursorRedraw(emboxVC.fb, mouseX, mouseY);
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
