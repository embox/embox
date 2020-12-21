#include <qgenericplugin_qpa.h>
#include "qemboxinput.h"

QT_BEGIN_NAMESPACE

class QEmboxInputPlugin : public QGenericPlugin
{
public:
	QEmboxInputPlugin();

	QStringList keys() const;
	QObject* create(const QString &key, const QString &specification);
};

QEmboxInputPlugin::QEmboxInputPlugin()
	: QGenericPlugin()
{
}

QStringList QEmboxInputPlugin::keys() const
{
	return (QStringList()
			<< QLatin1String("EmboxInputMouse")
			<< QLatin1String("EmboxInputKeyboard"));
}

QObject* QEmboxInputPlugin::create(const QString &key,
                                   const QString &specification)
{
	if (!key.compare(QLatin1String("EmboxInputMouse"), Qt::CaseInsensitive))
		return new QEmboxInputMouseHandler(key, specification);
	if (!key.compare(QLatin1String("EmboxInputKeyboard"), Qt::CaseInsensitive))
		return new QEmboxInputKeyboardHandler(key, specification);
	return 0;
}

Q_EXPORT_PLUGIN2(qemboxinputplugin, QEmboxInputPlugin)

QT_END_NAMESPACE
