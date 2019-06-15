#include <QPlatformIntegrationPlugin>
#include "emboxfbintegration.h"

QT_BEGIN_NAMESPACE

class QEmboxFbIntegrationPlugin : public QPlatformIntegrationPlugin
{
public:
	QStringList keys() const;
	QPlatformIntegration *create(const QString&, const QStringList&);
};

QStringList QEmboxFbIntegrationPlugin::keys() const
{
	QStringList list;
	list << "EmboxFb";
	return list;
}

QPlatformIntegration* QEmboxFbIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
	Q_UNUSED(paramList);
	if (system.toLower() == "emboxfb")
		return new QEmboxFbIntegration;

	return 0;
}

Q_EXPORT_PLUGIN2(emboxfb, QEmboxFbIntegrationPlugin)

QT_END_NAMESPACE
