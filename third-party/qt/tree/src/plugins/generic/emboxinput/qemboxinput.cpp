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

#include <drivers/input/input_dev.h>

QT_BEGIN_NAMESPACE

QEmboxInputMouseHandler::QEmboxInputMouseHandler(const QString &key,
	                                             const QString &specification)
	: m_notify(0), m_x(0), m_y(0), m_prevx(0), m_prevy(0), m_xoffset(0), m_yoffset(0), m_buttons(0), d(0)
{
	qDebug() << "QEmboxInputMouseHandler" << key << specification;

	setObjectName(QLatin1String("EmboxInputSubsystem Mouse Handler"));

	QString dev = QString();
	QStringList args = specification.split(QLatin1Char(':'));

	foreach (const QString &arg, args) {
		if (arg.startsWith(QLatin1String("/dev/"))) {
			dev = arg;
		}
	}

	m_fd = QT_OPEN(dev.toLocal8Bit().constData(), O_RDONLY | O_NONBLOCK, 0);
	if (m_fd >= 0) {
		m_notify = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
		connect(m_notify, SIGNAL(activated(int)), this, SLOT(readMouseData()));
	} else {
		qWarning("Cannot open mouse input device '%s': %s", qPrintable(dev), strerror(errno));
		return;
	}
}


QEmboxInputMouseHandler::~QEmboxInputMouseHandler()
{
	if (m_fd >= 0)
		QT_CLOSE(m_fd);
}

void QEmboxInputMouseHandler::sendMouseEvent(int x, int y, Qt::MouseButtons buttons)
{
	QPoint pos(x+m_xoffset, y+m_yoffset);
	//qDebug() << "readMouseData" << x+m_xoffset << ", " << y+m_yoffset;
	QWindowSystemInterface::handleMouseEvent(0, pos, pos, m_buttons);
	m_prevx = x;
	m_prevy = y;
}

void QEmboxInputMouseHandler::readMouseData()
{
	struct input_event ev;

	while (1) {
		if (read(m_fd, &ev, sizeof ev) <= 0) {
			return;
		}

#ifdef QT_EMBOX_TOUCHSCREEN
		switch (ev.type & ~TS_EVENT_NEXT) {
		case TS_TOUCH_1:
			m_x = (int16_t) ((ev.value >> 16) & 0xffff);
			m_y = (int16_t) (ev.value & 0xffff);

			m_buttons = Qt::LeftButton;

			break;
		case TS_TOUCH_1_RELEASED:
			m_buttons = Qt::NoButton;

			break;
		default:
			continue;
		}
#else
		if ((ev.type & MOUSE_BUTTON_PRESSED) == 0) { /* Mouse move */
			m_x += (int16_t) ((ev.value >> 16) & 0xffff);
			m_y -= (int16_t) (ev.value & 0xffff);
		}

		m_buttons = Qt::NoButton;

		if (ev.type & MOUSE_BUTTON_LEFT) {
			m_buttons |= Qt::LeftButton;
		}
		if (ev.type & MOUSE_BUTTON_MIDDLE) {
			m_buttons |= Qt::MidButton;
		}
		if (ev.type & MOUSE_BUTTON_RIGHT) {
			m_buttons |= Qt::RightButton;
		}
#endif

		sendMouseEvent(m_x, m_y, m_buttons);
	}
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
