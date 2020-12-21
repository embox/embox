#ifndef QEMBOXINPUT_H
#define QEMBOXINPUT_H

#include <qobject.h>
#include <Qt>
#include <termios.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSocketNotifier;

class QEmboxInputMouseHandlerData;

class QEmboxInputMouseHandler : public QObject
{
	Q_OBJECT
public:
	QEmboxInputMouseHandler(const QString &key, const QString &specification);
	~QEmboxInputMouseHandler();

private slots:
	void readMouseData();

private:
	void sendMouseEvent(int x, int y, Qt::MouseButtons buttons);
	QSocketNotifier *          m_notify;
	int                        m_fd;
	int                        m_x, m_y;
	int m_prevx, m_prevy;
	int m_xoffset, m_yoffset;
	Qt::MouseButtons           m_buttons;
	QEmboxInputMouseHandlerData *d;
};


class QWSEmboxInputKeyboardHandler;

class QEmboxInputKeyboardHandler : public QObject
{
	Q_OBJECT
public:
	QEmboxInputKeyboardHandler(const QString &key, const QString &specification);
	~QEmboxInputKeyboardHandler();


private:
	void switchLed(int, bool);

private slots:
	void readKeycode();

private:
	QWSEmboxInputKeyboardHandler *m_handler;
	int                           m_fd;
	int                           m_tty_fd;
	struct termios                m_tty_attr;
	int                           m_orig_kbmode;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QEMBOXINPUT_H
