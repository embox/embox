#include <QtGui>
#include "mdi_background.h"

void QMdiAreaWBackground::resizeEvent(QResizeEvent *reEvent)
{
	size = reEvent->size();
	resize(size);
	QMdiArea::resizeEvent(reEvent);
}

void QMdiAreaWBackground::resize(QSize& size)
{
	/* XXX Conversion of image to the ARGB32 format is placed here because of setBackroung() works on Embox correctly
	 * only if the image is not opaque. I've looked for the method in which Qt determines that image is opaque,
	 * and discovered that ARGB32 format is a workaround for this problem.
	 * Also it is not obviously for me why _opaque_ images is not drawn while not-opaque is drawn... -- Alexander */
	setBackground((i.scaled(size)).convertToFormat(QImage::Format_ARGB32));//,Qt::KeepAspectRatio);
}

void QMdiAreaWBackground::setBackgroundPath(QString& str) {
	i = QImage(str);
	resize(size);
}

QMdiAreaWBackground::QMdiAreaWBackground(QString& str)
{
	i = QImage(str);
}

