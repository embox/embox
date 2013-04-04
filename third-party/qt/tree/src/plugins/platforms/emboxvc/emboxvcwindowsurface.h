#ifndef QWINDOWSURFACE_MINIMAL_H
#define QWINDOWSURFACE_MINIMAL_H

#include "emboxvccursor.h"
#include <QtGui/private/qwindowsurface_p.h>
#include <QtCore/QSocketNotifier>
#include <QtGui/QPlatformWindow>

#include <drivers/video/fb.h>
#include <drivers/input/keymap.h>
#include <drivers/keyboard.h>
#include <drivers/console/mpx.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE

class QEmboxVCMouseHandler : public QObject
{
    Q_OBJECT
public:
    QEmboxVCMouseHandler();
    ~QEmboxVCMouseHandler();

    void storeData(void *data, int datalen);

private slots:
    void readMouseData();

private:
    int mouseFD, inputFD;
    QSocketNotifier *mouseNotifier;
};

class QEmboxVCKeyboardHandler : public QObject
{
    Q_OBJECT
public:
    QEmboxVCKeyboardHandler();
    ~QEmboxVCKeyboardHandler();

    void storeData(void *data, int datalen);

private slots:
    void readKeyboardData();

private:
    int keyboardFD, inputFD;
    QSocketNotifier *keyboardNotifier;
};

class QEmboxVC
{
public:
	QEmboxVC();
    ~QEmboxVC();

    struct vc emboxVC;
    int emboxVCvisualized;
    QEmboxCursor *cursor;
    int mouseX, mouseY;
    QEmboxVCMouseHandler *mouseHandler;
    QEmboxVCKeyboardHandler *keyboardHandler;

private:
    struct vc_callbacks emboxVCcallbacks;
};

class QEmboxVCPlatformWindow : public QPlatformWindow
{
public:
	QEmboxVCPlatformWindow(QWidget *widget);
	~QEmboxVCPlatformWindow();

    WId winId() const;
    void setParent(const QPlatformWindow *window);
};

class QEmboxVCWindowSurface : public QWindowSurface
{
public:
    QEmboxVCWindowSurface(QWidget *window);
    ~QEmboxVCWindowSurface();

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

    QEmboxVC *vc;

private:
    QImage mImage;
    struct vc_callbacks emboxVCcallbacks;
};

QT_END_NAMESPACE

#endif
