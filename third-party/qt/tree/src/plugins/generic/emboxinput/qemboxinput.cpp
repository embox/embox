#include "qemboxinput.h"


#include <QSocketNotifier>
#include <QStringList>
#include <QPoint>
#include <QWindowSystemInterface>

#include <qkbd_qws.h>


#include <qplatformdefs.h>
#include <private/qcore_unix_p.h> // overrides QT_OPEN

#include <errno.h>
#include <termios.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QEmboxInputMouseHandler::QEmboxInputMouseHandler(const QString &key,
	                                             const QString &specification)
	: m_notify(0), m_x(0), m_y(0), m_prevx(0), m_prevy(0), m_xoffset(0), m_yoffset(0), m_buttons(0), d(0)
{
	qDebug() << "QEmboxInputMouseHandler" << key << specification;
}


QEmboxInputMouseHandler::~QEmboxInputMouseHandler()
{
	if (m_fd >= 0)
		QT_CLOSE(m_fd);
}

void QEmboxInputMouseHandler::sendMouseEvent(int x, int y, Qt::MouseButtons buttons)
{
	QPoint pos(x+m_xoffset, y+m_yoffset);
	QWindowSystemInterface::handleMouseEvent(0, pos, pos, m_buttons);
	m_prevx = x;
	m_prevy = y;
}

void QEmboxInputMouseHandler::readMouseData()
{
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Keyboard handler




class QWSEmboxInputKeyboardHandler : public QWSKeyboardHandler
{
public:
	QWSEmboxInputKeyboardHandler(const QString&);
	~QWSEmboxInputKeyboardHandler();

	virtual bool filterInputEvent(quint16 &input_code, qint32 &input_value);

//private:
//	QWSEmboxInputKbPrivate *d;
};


QWSEmboxInputKeyboardHandler::QWSEmboxInputKeyboardHandler(const QString &device)
	: QWSKeyboardHandler(device)
{
}

QWSEmboxInputKeyboardHandler::~QWSEmboxInputKeyboardHandler()
{
}

bool QWSEmboxInputKeyboardHandler::filterInputEvent(quint16 &, qint32 &)
{
	return false;
}


QEmboxInputKeyboardHandler::QEmboxInputKeyboardHandler(const QString &key, const QString &specification)
	: m_handler(0), m_fd(-1), m_tty_fd(-1), m_orig_kbmode(0)
{
}

QEmboxInputKeyboardHandler::~QEmboxInputKeyboardHandler()
{
	if (m_fd >= 0)
		QT_CLOSE(m_fd);
}

void QEmboxInputKeyboardHandler::switchLed(int led, bool state)
{
}



void QEmboxInputKeyboardHandler::readKeycode()
{
}

QT_END_NAMESPACE
