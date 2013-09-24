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
        setBackground(i.scaled(size));//,Qt::KeepAspectRatio);
}

void QMdiAreaWBackground::setBackgroundPath(QString& str) {
        i = QImage(str);
        resize(size);
}

QMdiAreaWBackground::QMdiAreaWBackground(QString& str)
{
        i = QImage(str);
}

